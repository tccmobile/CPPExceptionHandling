This example demonstrates the key features of C++ exception handling. Here are the main components:

1. Custom Exception Class:
   - Inherits from std::runtime_error
   - Implements nested exception handling
   - Uses smart pointers for exception nesting

2. Resource Management:
   - Demonstrates RAII pattern
   - Shows proper move semantics
   - Implements proper cleanup in destructors
   - Uses smart pointers for automatic resource management

3. Exception Handling Features:
   - Basic try-catch blocks
   - Multiple catch blocks with exception hierarchy
   - noexcept specifications
   - Template functions with exception specifications
   - Nested exceptions
   - Scope-based resource management

4. Modern C++ Features:
   - Smart pointers (std::unique_ptr, std::shared_ptr)
   - Move semantics
   - Template programming
   - Standard library exceptions



**RAII** stands for "Resource Acquisition Is Initialization," and it's a C++ programming pattern where resource management is tied to object lifetime. Here's how it works:

1. Core Concept:
   ```cpp
   class ResourceOwner {
       Resource* ptr;
   public:
       // Constructor acquires the resource
       ResourceOwner() : ptr(new Resource()) { }
       
       // Destructor releases the resource
       ~ResourceOwner() { delete ptr; }
   };
   ```
   When you create an object, you acquire resources in its constructor. When the object goes out of scope, its destructor automatically releases those resources.

2. Why RAII is Important:
   ```cpp
   void withoutRAII() {
       Resource* ptr = new Resource();
       // If an exception occurs here, ptr will leak
       doSomething();
       delete ptr;  // Might never get here
   }

   void withRAII() {
       ResourceOwner owner;  // Resource acquired
       doSomething();
       // Even if an exception occurs, destructor will release resource
   }  // Resource automatically released
   ```

3. Common RAII Use Cases:

   a. Smart Pointers:
   ```cpp
   void example() {
       std::unique_ptr<MyClass> ptr(new MyClass());
       // ptr automatically deleted when function ends
   }
   ```

   b. File Handling:
   ```cpp
   class FileHandler {
       std::ifstream file;
   public:
       FileHandler(const std::string& filename) : file(filename) { }
       // File automatically closed when FileHandler is destroyed
   };
   ```

   c. Mutex Locks:
   ```cpp
   void threadSafeOperation() {
       std::lock_guard<std::mutex> lock(mutex);
       // Critical section code here
       // Mutex automatically unlocked when lock_guard is destroyed
   }
   ```

4. Benefits:
   - Exception Safety: Resources are released even if exceptions occur
   - Prevention of Resource Leaks: Can't forget to release resources
   - Clear Resource Lifetime: Resources tied to scope
   - Self-Documenting Code: Resource management is explicit in class design

5. Modern C++ Example combining multiple RAII concepts:
   ```cpp
   class DatabaseConnection {
       std::unique_ptr<Connection> conn;
       std::lock_guard<std::mutex> lock;
       std::ofstream log_file;
   
   public:
       DatabaseConnection(const std::string& db_name)
           : conn(createConnection(db_name))
           , lock(connection_mutex)
           , log_file("transaction.log")
       {
           // All resources acquired in initializer list
       }
       // All resources automatically released when object is destroyed
   };
   ```

6. Key Guidelines:
   - Never manually manage resources if RAII can be used
   - Keep constructors simple and exception-safe
   - Don't throw exceptions in destructors
   - Use existing RAII types (smart pointers, containers) when possible
   - Make classes non-copyable if they own exclusive resources

7. Common Standard Library RAII Types:
   ```cpp
   std::unique_ptr<T>        // Exclusive ownership
   std::shared_ptr<T>        // Shared ownership
   std::lock_guard<mutex>    // Mutex management
   std::unique_lock<mutex>   // Flexible mutex management
   std::scoped_lock         // Multiple mutex management
   std::vector<T>           // Dynamic array
   std::string             // Dynamic string
   ```

The power of RAII comes from its automatic nature - you don't need to remember to free resources or write cleanup code. The C++ scope and lifetime rules handle it automatically. This makes C++ code more robust and less prone to resource leaks compared to languages that rely on garbage collection or manual resource management.



