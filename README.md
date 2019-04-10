# thread_work
something about thread


useage and sync task

```c++
thread_mgr::init();

// add worker threads
thread_mgr::add_worker<thread::render>();

thread_mgr::instance().sync<thread::logic>(
	[]() {
		std::cout << "task 1" << std::endl;
	},
	[]() {
		std::cout << "task 2" << std::endl;
	}
	thread::work_at<thread::render>([]() {
		std::cout << "task3 work in render thread" << std::endl;
	}),

	[]() {
		std::cout << "task 4 working in logic" << std::endl;
	}
)

thread_mgr::tick<thread::logic>();
```


mixed sync and async
```c++

thread_mgr::sync<thread::logic>(
	[]() {
		std::cout << "task 1" << std::endl;
	},
	[](thrad_context &context) {
		std::cout << "task 2" << std::endl;

		for (int i = 0; i < 10; ++i ) {
			context.async<thread::any>([=]() {
				std::cout << "task " << i << " in any thread" << std::endl;
			});
		}
	},
	[]() {
		std::cout << "task 3" << std::endl;
	}
)

```