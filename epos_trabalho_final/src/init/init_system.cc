// EPOS System Initializer

#include <utility/random.h>
#include <machine.h>
#include <system.h>
#include <address_space.h>
#include <segment.h>

__BEGIN_SYS

class Init_System
{
private:
    static const unsigned int HEAP_SIZE = Traits<System>::HEAP_SIZE;

public:
    Init_System() {
		Machine::smp_init(System::info()->bm.n_cpus);
        db<Init>(TRC) << "Init_System()" << endl;

		if(Machine::cpu_id() == 0){
			CPU::int_disable();

			// Initialize the processor
			db<Init>(INF) << "Initializing the CPU: " << endl;
			CPU::init();
			db<Init>(INF) << "done!" << endl;

			// Initialize System's heap
			db<Init>(INF) << "Initializing system's heap: " << endl;
			if(Traits<System>::multiheap) {
				System::_heap_segment = new (&System::_preheap[0]) Segment(HEAP_SIZE, Segment::Flags::SYS);
				System::_heap = new (&System::_preheap[sizeof(Segment)]) Heap(Address_Space(MMU::current()).attach(System::_heap_segment, Memory_Map<Machine>::SYS_HEAP), System::_heap_segment->size());
			} else
				System::_heap = new (&System::_preheap[0]) Heap(MMU::alloc(MMU::pages(HEAP_SIZE)), HEAP_SIZE);
			db<Init>(INF) << "done!" << endl;

			// Initialize the machine
			db<Init>(INF) << "Initializing the machine: " << endl;
			Machine::init();
			db<Init>(INF) << "done!" << endl;

			Machine::smp_barrier();

			// Initialize system abstractions
			db<Init>(INF) << "Initializing system abstractions: " << endl;
			System::init();
			db<Init>(INF) << "done!" << endl;

			// Randomize the Random Numbers Generator's seed
			if(Traits<Random>::enabled) {
				db<Init>(INF) << "Randomizing the Random Numbers Generator's seed: " << endl;
				if(Traits<TSC>::enabled)
					Random::seed(TSC::time_stamp());

				if(!Traits<TSC>::enabled)
					db<Init>(WRN) << "Due to lack of entropy, Random is a pseudo random numbers generator!" << endl;
				db<Init>(INF) << "done!" << endl;
			}
        }else {
        	Machine::smp_barrier();
        	Timer::init();
        }

        // Initialization continues at init_first
    }
};

// Global object "init_system" must be constructed first.
Init_System init_system;

__END_SYS
