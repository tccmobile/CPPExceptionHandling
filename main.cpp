#include <iostream>
#include <stdexcept>
#include <memory>
#include <string>

// Custom exception class
class CustomResourceException : public std::runtime_error {
public:
    explicit CustomResourceException(const std::string& message)
        : std::runtime_error(message), nested_ptr_(nullptr) {}

    CustomResourceException(const std::string& message, const std::exception& nested)
        : std::runtime_error(message), nested_ptr_(std::make_shared<std::exception>(nested)) {}

    const std::exception* nested() const {
        return nested_ptr_ ? nested_ptr_.get() : nullptr;
    }

private:
    std::shared_ptr<std::exception> nested_ptr_;
};

// Resource class demonstrating RAII (Resource Acquisition Is Initialization)
class Resource {
public:
    explicit Resource(std::string name) : name_(std::move(name)), is_open_(true) {
        std::cout << "Resource " << name_ << " opened\n";
    }

    // Destructor - automatically called when object goes out of scope
    ~Resource() {
        try {
            close();
        } catch (const CustomResourceException& e) {
            std::cerr << "Destructor caught exception: " << e.what() << '\n';
        }
    }

    // Disable copying to prevent resource leaks
    Resource(const Resource&) = delete;
    Resource& operator=(const Resource&) = delete;

    // Enable moving
    Resource(Resource&& other) noexcept
        : name_(std::move(other.name_)), is_open_(other.is_open_) {
        other.is_open_ = false;
    }

    Resource& operator=(Resource&& other) noexcept {
        if (this != &other) {
            name_ = std::move(other.name_);
            is_open_ = other.is_open_;
            other.is_open_ = false;
        }
        return *this;
    }

    void performOperation() {
        if (!is_open_) {
            throw CustomResourceException("Resource " + name_ + " is closed");
        }
        if (name_ == "faulty") {
            throw CustomResourceException("Resource " + name_ + " is faulty");
        }
        std::cout << "Operation performed on resource " << name_ << '\n';
    }

    void close() {
        if (is_open_) {
            is_open_ = false;
            std::cout << "Resource " << name_ << " closed\n";
            if (name_ == "failing") {
                throw CustomResourceException("Failed to close resource " + name_);
            }
        }
    }

private:
    std::string name_;
    bool is_open_;
};

// Function demonstrating nested exceptions
void demonstrateNestedExceptions() {
    try {
        throw std::runtime_error("Original error");
    } catch (const std::exception& e) {
        throw CustomResourceException("Wrapper error", e);
    }
}

// Function template with noexcept specification
template<typename T>
T divide(T a, T b) noexcept(false) {
    if (b == 0) {
        throw std::invalid_argument("Division by zero");
    }
    return a / b;
}

int main() {
    try {
        // 1. Basic try-catch with smart pointer
        std::cout << "\n1. Smart Pointer Example:\n";
        auto resource = std::make_unique<Resource>("basic");
        resource->performOperation();

        // 2. Multiple catch blocks
        std::cout << "\n2. Multiple Catch Blocks:\n";
        try {
            throw std::runtime_error("Runtime error");
        } catch (const std::invalid_argument& e) {
            std::cout << "Caught invalid argument: " << e.what() << '\n';
        } catch (const std::exception& e) {
            std::cout << "Caught exception: " << e.what() << '\n';
        }

        // 3. RAII and scope-based resource management
        std::cout << "\n3. RAII Example:\n";
        {
            Resource r1("r1");
            r1.performOperation();
            // r1 will be automatically closed when it goes out of scope
        }

        // 4. Exception specification and templates
        std::cout << "\n4. Exception Specification:\n";
        try {
            int result = divide(10, 0);
        } catch (const std::invalid_argument& e) {
            std::cout << "Division error: " << e.what() << '\n';
        }

        // 5. Nested exceptions
        std::cout << "\n5. Nested Exceptions:\n";
        try {
            demonstrateNestedExceptions();
        } catch (const CustomResourceException& e) {
            std::cout << "Main error: " << e.what() << '\n';
            if (const std::exception* nested = e.nested()) {
                std::cout << "Nested error: " << nested->what() << '\n';
            }
        }

        // 6. Multiple resources and exceptions
        std::cout << "\n6. Multiple Resources:\n";
        try {
            Resource r2("faulty");
            Resource r3("failing");
            r2.performOperation();
            r3.performOperation();
        } catch (const CustomResourceException& e) {
            std::cout << "Resource error: " << e.what() << '\n';
        }

    } catch (const std::exception& e) {
        std::cout << "Top-level catch: " << e.what() << '\n';
    }

    return 0;
}