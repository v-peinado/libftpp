#include "libftpp.hpp"
#include <iostream>
#include <cassert>
#include <string>

/**
 * NOTA IMPORTANTE SOBRE EL POOL:
 * El Pool requiere que los objetos tengan un constructor por defecto
 * porque resize() crea los objetos inicialmente con new TType().
 * Luego, acquire() destruye el objeto y lo reconstruye con los parámetros dados.
 * Recordemos que es una Pool de objetos
 */

// ============================================================================
// CLASE DE TEST SIMPLE - La misma del test original
// ============================================================================
class TestObject {
public:
    TestObject() { 
        std::cout << "TestObject default constructor" << std::endl; 
    }
    
    TestObject(int value) { 
        std::cout << "TestObject constructor with value [" << value << "]" << std::endl;
    }
    
    ~TestObject() { 
        std::cout << "TestObject destructor" << std::endl; 
    }

    void sayHello() const { 
        std::cout << "Hello from TestObject" << std::endl; 
    }
};

// ============================================================================
// TEST ORIGINAL - Sin modificar
// ============================================================================
void test_original() {
    std::cout << "\n=== TEST ORIGINAL (sin cambios) ===" << std::endl;
    
    // Create a Pool for TestObject
    Pool<TestObject> myPool;

    // Resize the pool to pre-allocate 5 objects
    // Should output the 5 "TestObject constructor"
    myPool.resize(5);

    // Acquire an object from the pool
    Pool<TestObject>::Object obj1 = myPool.acquire(15);
    obj1->sayHello();  // Should output: "Hello from TestObject"

    {
        // Acquire another object in a different scope
        Pool<TestObject>::Object obj2 = myPool.acquire();
        obj2->sayHello();  // Should also output: "Hello from TestObject"
        // obj2 is released back to the pool when it goes out of scope
    }

    // Acquire another object; this should give us the object that obj2 pointed to
    Pool<TestObject>::Object obj3 = myPool.acquire();
    obj3->sayHello();  // Should output: "Hello from TestObject"

    // obj1 and obj3 are released back to the pool when the program ends
}

// ============================================================================
// TESTS ADICIONALES - Comentados exhaustivamente
// ============================================================================

/**
 * TEST 1: Verificación de Pool Vacío
 * 
 * ¿POR QUÉ ES IMPORTANTE?
 * - El enunciado especifica que acquire() debe lanzar std::runtime_error si el pool está vacío
 * - Necesitamos verificar que el manejo de errores funciona correctamente
 * - Un pool sin resize() no debe permitir acquire()
 * 
 * ¿QUÉ PROBAMOS?
 * - Que se lance la excepción correcta cuando no hay objetos disponibles
 * - Que el mensaje de error sea descriptivo
 */
void test_empty_pool() {
    std::cout << "\n=== TEST 1: Pool Vacío (manejo de errores) ===" << std::endl;
    
    Pool<TestObject> emptyPool;  // Pool sin resize()
    
    try {
        // Esto DEBE fallar porque no hemos llamado a resize()
        auto obj = emptyPool.acquire();
        
        // Si llegamos aquí, el test falló - no se lanzó excepción
        std::cout << "❌ ERROR: No se lanzó excepción!" << std::endl;
        assert(false);
    } 
    catch (const std::runtime_error& e) {
        // Esto es lo esperado según el enunciado
        std::cout << "✅ Excepción correcta: " << e.what() << std::endl;
    }
}

/**
 * TEST 2: Métodos Informativos del Pool
 * 
 * ¿POR QUÉ ES IMPORTANTE?
 * - Los métodos size(), available(), inUse(), isEmpty(), isFull() son parte del header
 * - Necesitamos verificar que reportan el estado correcto del pool
 * - Esto es crítico para que el usuario pueda monitorear el pool
 * 
 * ¿QUÉ PROBAMOS?
 * - Que los contadores se actualicen correctamente con acquire/release
 * - Que isEmpty() y isFull() reflejen el estado real
 */
void test_pool_state_methods() {
    std::cout << "\n=== TEST 2: Métodos de Estado del Pool ===" << std::endl;
    
    Pool<TestObject> pool;
    pool.resize(3);  // Pool pequeño para facilitar el test
    
    // Estado inicial - todos los objetos disponibles
    std::cout << "Estado inicial:" << std::endl;
    std::cout << "  size(): " << pool.size() << " (esperado: 3)" << std::endl;
    std::cout << "  available(): " << pool.available() << " (esperado: 3)" << std::endl;
    std::cout << "  inUse(): " << pool.inUse() << " (esperado: 0)" << std::endl;
    std::cout << "  isEmpty(): " << pool.isEmpty() << " (esperado: false)" << std::endl;
    std::cout << "  isFull(): " << pool.isFull() << " (esperado: true)" << std::endl;
    
    // Verificamos con assert (detiene el programa si falla)
    assert(pool.size() == 3);
    assert(pool.available() == 3);
    assert(pool.inUse() == 0);
    assert(pool.isEmpty() == false);
    assert(pool.isFull() == true);
    
    // Adquirimos un objeto
    auto obj1 = pool.acquire();
    
    std::cout << "\nDespués de acquire():" << std::endl;
    std::cout << "  available(): " << pool.available() << " (esperado: 2)" << std::endl;
    std::cout << "  inUse(): " << pool.inUse() << " (esperado: 1)" << std::endl;
    
    assert(pool.available() == 2);
    assert(pool.inUse() == 1);
    
    std::cout << "✅ Métodos de estado funcionan correctamente" << std::endl;
}

/**
 * TEST 3: Liberación Automática (RAII)
 * 
 * ¿POR QUÉ ES IMPORTANTE?
 * - El Pool::Object debe implementar RAII correctamente
 * - Cuando un Object sale de scope, debe retornar automáticamente al pool
 * - Esto previene memory leaks y simplifica el uso
 * 
 * ¿QUÉ PROBAMOS?
 * - Que el destructor de Pool::Object llame a returnToPool()
 * - Que el índice se restaure en m_available
 */
void test_automatic_release() {
    std::cout << "\n=== TEST 3: Liberación Automática (RAII) ===" << std::endl;
    
    Pool<TestObject> pool;
    pool.resize(2);
    
    std::cout << "Objetos disponibles inicialmente: " << pool.available() << std::endl;
    
    {
        std::cout << "Entrando en scope anidado..." << std::endl;
        auto obj = pool.acquire();
        std::cout << "Objetos disponibles después de acquire: " << pool.available() << std::endl;
        assert(pool.available() == 1);
        
        // obj se destruye automáticamente aquí
    }
    
    std::cout << "Salimos del scope anidado..." << std::endl;
    std::cout << "Objetos disponibles después del scope: " << pool.available() << std::endl;
    
    // El objeto debe haberse liberado automáticamente
    assert(pool.available() == 2);
    std::cout << "✅ RAII funciona - liberación automática confirmada" << std::endl;
}

/**
 * TEST 4: Agotamiento del Pool
 * 
 * ¿POR QUÉ ES IMPORTANTE?
 * - Necesitamos verificar qué pasa cuando pedimos más objetos de los disponibles
 * - El comportamiento debe ser predecible y seguro
 * 
 * ¿QUÉ PROBAMOS?
 * - Que podamos adquirir exactamente N objetos si el pool tiene tamaño N
 * - Que el objeto N+1 lance excepción
 */
void test_pool_exhaustion() {
    std::cout << "\n=== TEST 4: Agotamiento del Pool ===" << std::endl;
    
    Pool<TestObject> pool;
    pool.resize(2);  // Solo 2 objetos
    
    // Adquirimos todos los objetos disponibles
    auto obj1 = pool.acquire();
    auto obj2 = pool.acquire();
    
    std::cout << "Pool agotado - isEmpty(): " << pool.isEmpty() << std::endl;
    assert(pool.isEmpty() == true);
    
    try {
        // Esto debe fallar - no hay más objetos
        auto obj3 = pool.acquire();
        std::cout << "❌ ERROR: Se permitió acquire con pool vacío!" << std::endl;
        assert(false);
    }
    catch (const std::runtime_error& e) {
        std::cout << "✅ Excepción correcta al agotar pool: " << e.what() << std::endl;
    }
}

/**
 * TEST 5: Move Semantics de Pool::Object
 * 
 * ¿POR QUÉ ES IMPORTANTE?
 * - Pool::Object debe ser movible pero no copiable (según el código)
 * - El move debe transferir ownership correctamente
 * - Solo un Object debe ser responsable de devolver el recurso al pool
 * 
 * ¿QUÉ PROBAMOS?
 * - Que el move constructor funcione
 * - Que el move assignment funcione y libere el objeto anterior
 * - Que isValid() reporte el estado correcto
 */
void test_move_semantics() {
    std::cout << "\n=== TEST 5: Move Semantics ===" << std::endl;
    
    Pool<TestObject> pool;
    pool.resize(3);
    
    // Test del move constructor
    std::cout << "\nProbando move constructor:" << std::endl;
    {
        auto obj1 = pool.acquire();
        std::cout << "obj1 válido: " << obj1.isValid() << std::endl;
        
        // Move constructor - transferimos ownership
        auto obj2 = std::move(obj1);
        
        std::cout << "Después del move:" << std::endl;
        std::cout << "  obj1 válido: " << obj1.isValid() << " (debe ser false)" << std::endl;
        std::cout << "  obj2 válido: " << obj2.isValid() << " (debe ser true)" << std::endl;
        
        assert(!obj1.isValid());  // obj1 ya no es válido
        assert(obj2.isValid());   // obj2 ahora tiene el objeto
        
        // Solo debe haber 1 objeto en uso
        assert(pool.inUse() == 1);
    }
    
    // Test del move assignment
    std::cout << "\nProbando move assignment:" << std::endl;
    {
        auto obj1 = pool.acquire();
        auto obj2 = pool.acquire();
        
        std::cout << "Antes del move assignment - objetos en uso: " << pool.inUse() << std::endl;
        assert(pool.inUse() == 2);
        
        // Move assignment - obj1 debe liberar su objeto actual y tomar el de obj2
        obj1 = std::move(obj2);
        
        std::cout << "Después del move assignment - objetos en uso: " << pool.inUse() << std::endl;
        
        // Ahora solo debe haber 1 objeto en uso (el anterior de obj1 se liberó)
        assert(pool.inUse() == 1);
        assert(!obj2.isValid());
        assert(obj1.isValid());
    }
    
    std::cout << "✅ Move semantics funcionan correctamente" << std::endl;
}

/**
 * TEST 6: Perfect Forwarding (Avanzado)
 * 
 * ¿POR QUÉ ES IMPORTANTE?
 * - El Pool usa templates variadicos y std::forward para perfect forwarding
 * - Esto permite que los argumentos lleguen al constructor con su categoría original
 * - Sin perfect forwarding, todo llegaría como lvalue (copias innecesarias)
 * 
 * ¿QUÉ PROBAMOS?
 * - Que los argumentos se forwarden correctamente al constructor
 * - Que funcione con diferentes números de parámetros
 * 
 * NOTA IMPORTANTE: ComplexObject necesita constructor por defecto porque
 * Pool::resize() crea los objetos inicialmente con new TType()
 * Luego, acquire() destruye y reconstruye con los parámetros deseados
 */
class ComplexObject {
public:
    std::string data;
    
    // Constructor por defecto - NECESARIO para Pool::resize()
    // El Pool necesita poder crear objetos vacíos al hacer resize
    ComplexObject() : data("default") {
        std::cout << "ComplexObject constructor por defecto" << std::endl;
    }
    
    // Constructor que acepta string por valor
    ComplexObject(std::string s) : data(std::move(s)) {
        std::cout << "ComplexObject creado con: " << data << std::endl;
    }
    
    // Constructor con múltiples parámetros
    ComplexObject(int x, const std::string& s) : data(s + " " + std::to_string(x)) {
        std::cout << "ComplexObject creado con int y string: " << data << std::endl;
    }
    
    // Destructor para ver el ciclo de vida
    ~ComplexObject() {
        std::cout << "ComplexObject destruido: " << data << std::endl;
    }
};

void test_perfect_forwarding() {
    std::cout << "\n=== TEST 6: Perfect Forwarding (Avanzado) ===" << std::endl;
    
    Pool<ComplexObject> pool;
    
    std::cout << "\nPaso 1: resize() crea objetos con constructor por defecto:" << std::endl;
    pool.resize(3);  // Verás 3 llamadas al constructor por defecto
    
    std::cout << "\nPaso 2: acquire() destruye y reconstruye con los parámetros dados:" << std::endl;
    
    // Test 1: Forwarding con rvalue (move)
    std::cout << "\nTest con rvalue (debe usar move):" << std::endl;
    auto obj1 = pool.acquire(std::string("temporal"));
    
    // Test 2: Forwarding con múltiples parámetros
    std::cout << "\nTest con múltiples parámetros:" << std::endl;
    auto obj2 = pool.acquire(42, "respuesta");
    
    std::cout << "\n✅ Perfect forwarding funciona - los argumentos se pasan correctamente" << std::endl;
    std::cout << "NOTA: Verás destructores cuando los objetos se liberan al final" << std::endl;
}

// ============================================================================
// FUNCIÓN PRINCIPAL
// ============================================================================
int main() {
    std::cout << "=====================================" << std::endl;
    std::cout << "   TESTS COMPLETOS DEL POOL" << std::endl;
    std::cout << "=====================================" << std::endl;
    
    // Primero ejecutamos el test original sin cambios
    test_original();
    
    std::cout << "\n=====================================" << std::endl;
    std::cout << "   TESTS ADICIONALES" << std::endl;
    std::cout << "=====================================" << std::endl;
    
    // Luego ejecutamos los tests adicionales que verifican características específicas
    try {
        test_empty_pool();           // Verifica manejo de errores
        test_pool_state_methods();   // Verifica métodos informativos
        test_automatic_release();    // Verifica RAII
        test_pool_exhaustion();      // Verifica límites
        test_move_semantics();       // Verifica move operations
        test_perfect_forwarding();   // Verifica perfect forwarding
        
        std::cout << "\n=====================================" << std::endl;
        std::cout << "✅ TODOS LOS TESTS PASARON ✅" << std::endl;
        std::cout << "=====================================" << std::endl;
    }
    catch (const std::exception& e) {
        std::cerr << "\n❌ TEST FALLÓ: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}