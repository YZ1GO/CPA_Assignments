#include "lu.hpp"
#include <iostream>
#include <limits>
#include <stdexcept>
#include <string>
#include <vector>
#include <sycl/sycl.hpp>

namespace sycl_ns = sycl;

namespace {

void print_device_info(const sycl_ns::device& dev, const std::string& selector_name) {
    std::cout << selector_name << ":\n"
              << "      Selected device: " << dev.get_info<sycl_ns::info::device::name>() << "\n"
              << "       -> Device vendor: " << dev.get_info<sycl_ns::info::device::vendor>() << "\n"
              << "      type:       "
              << (dev.is_gpu() ? "GPU" : (dev.is_cpu() ? "CPU" : (dev.is_accelerator() ? "Accelerator" : "Other")))
              << "\n"
              << "      version:    " << dev.get_info<sycl_ns::info::device::version>() << "\n"
              << "      driver:     " << dev.get_info<sycl_ns::info::device::driver_version>() << "\n";
}

void print_all_device_selectors() {
    std::cout << "\n=== Available Device Selectors ===\n";
    
    try {
        sycl_ns::device dev{sycl_ns::default_selector()};
        print_device_info(dev, "default_selector");
    } catch (...) {
        std::cout << "default_selector: (not available)\n";
    }
    
    try {
        sycl_ns::device dev{sycl_ns::host_selector()};
        print_device_info(dev, "host_selector");
    } catch (...) {
        std::cout << "host_selector: (not available)\n";
    }
    
    try {
        sycl_ns::device dev{sycl_ns::cpu_selector()};
        print_device_info(dev, "cpu_selector");
    } catch (...) {
        std::cout << "cpu_selector: (not available)\n";
    }
    
    try {
        sycl_ns::device dev{sycl_ns::gpu_selector()};
        print_device_info(dev, "gpu_selector");
    } catch (...) {
        std::cout << "gpu_selector: (not available)\n";
    }
    
    try {
        sycl_ns::device dev{sycl_ns::accelerator_selector()};
        print_device_info(dev, "accelerator_selector");
    } catch (...) {
        std::cout << "accelerator_selector: (not available)\n";
    }
    
    try {
        #ifdef INTEL_FPGA_SELECTOR_AVAILABLE
        sycl_ns::device dev{sycl_ns::INTEL::fpga_selector()};
        #else
        // Fallback for systems without INTEL FPGA selector
        sycl_ns::device dev{sycl_ns::accelerator_selector()};
        #endif
        print_device_info(dev, "fpga_selector");
    } catch (...) {
        std::cout << "fpga_selector: (not available)\n";
    }
}

sycl_ns::device choose_device_once() {
    static bool selected = false;
    static sycl_ns::device chosen;

    if (selected) {
        return chosen;
    }

    print_all_device_selectors();

    // Test which selectors are available
    std::vector<bool> available(7, false);
    std::vector<std::string> selector_names = {"", "CPU", "GPU", "Accelerator", "Host", "Default", "FPGA"};
    
    for (int i = 1; i <= 6; ++i) {
        try {
            switch (i) {
                case 1:
                    sycl_ns::device{sycl_ns::cpu_selector()};
                    break;
                case 2:
                    sycl_ns::device{sycl_ns::gpu_selector()};
                    break;
                case 3:
                    sycl_ns::device{sycl_ns::accelerator_selector()};
                    break;
                case 4:
                    sycl_ns::device{sycl_ns::host_selector()};
                    break;
                case 5:
                    sycl_ns::device{sycl_ns::default_selector()};
                    break;
                case 6:
                    #ifdef INTEL_FPGA_SELECTOR_AVAILABLE
                    sycl_ns::device{sycl_ns::INTEL::fpga_selector()};
                    #else
                    sycl_ns::device{sycl_ns::accelerator_selector()};
                    #endif
                    break;
            }
            available[i] = true;
        } catch (...) {
            available[i] = false;
        }
    }

    // Show full menu
    std::cout << "\n=== Device Type Selection ===\n"
              << "1. CPU selector\n"
              << "2. GPU selector\n"
              << "3. Accelerator selector\n"
              << "4. Host selector\n"
              << "5. Default selector\n"
              << "6. FPGA selector\n";

    int selector_choice = -1;
    while (true) {
        std::cout << "Select device type [1-6]: ";
        if (std::cin >> selector_choice && selector_choice >= 1 && selector_choice <= 6) {
            if (available[selector_choice]) {
                break;
            } else {
                std::cout << "Device type " << selector_names[selector_choice] << " not available.\n";
                std::cout << "Please pick the available ones:\n";
                for (int i = 1; i <= 6; ++i) {
                    if (available[i]) {
                        std::cout << i << ". " << selector_names[i] << " selector\n";
                    }
                }
            }
        } else {
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            std::cout << "Invalid selection. Please try again.\n";
        }
    }

    // User picked an available device
    try {
        std::string device_type;
        switch (selector_choice) {
            case 1:
                chosen = sycl_ns::device{sycl_ns::cpu_selector()};
                device_type = "CPU";
                break;
            case 2:
                chosen = sycl_ns::device{sycl_ns::gpu_selector()};
                device_type = "GPU";
                break;
            case 3:
                chosen = sycl_ns::device{sycl_ns::accelerator_selector()};
                device_type = "Accelerator";
                break;
            case 4:
                chosen = sycl_ns::device{sycl_ns::host_selector()};
                device_type = "Host";
                break;
            case 5:
                chosen = sycl_ns::device{sycl_ns::default_selector()};
                device_type = "Default";
                break;
            case 6:
                #ifdef INTEL_FPGA_SELECTOR_AVAILABLE
                chosen = sycl_ns::device{sycl_ns::INTEL::fpga_selector()};
                #else
                chosen = sycl_ns::device{sycl_ns::accelerator_selector()};
                #endif
                device_type = "FPGA";
                break;
        }
        std::cout << "\n=== Selected Device (" << device_type << ") ===\n";
        std::cout << "Name: " << chosen.get_info<sycl_ns::info::device::name>() << "\n"
                  << "Vendor: " << chosen.get_info<sycl_ns::info::device::vendor>() << "\n";
        selected = true;
        return chosen;
    } catch (...) {
        throw std::runtime_error("Failed to select device.");
    }
}

} // namespace

bool lu_sycl_available() {
    return true;
}

void init_sycl_device() {
    // Initialize device selection before timing starts
    choose_device_once();
}

void lu_sycl(Matrix& A) {
    const int n = A.n;
    if (n <= 0) {
        return;
    }

    const sycl_ns::device dev = choose_device_once();
    sycl_ns::queue q(dev);

    std::cout << "SYCL queue running on: "
              << q.get_device().get_info<sycl_ns::info::device::name>()
              << "\n";

    sycl_ns::buffer<double, 1> a_buf(A.data.data(), sycl_ns::range<1>(static_cast<std::size_t>(n) * static_cast<std::size_t>(n)));

    for (int k = 0; k < n; ++k) {
        // Column normalization: A(i,k) /= A(k,k) for i>k
        if (k + 1 < n) {
            q.submit([&](sycl_ns::handler& h) {
                auto a = a_buf.template get_access<sycl_ns::access::mode::read_write>(h);
                h.parallel_for(sycl_ns::range<1>(static_cast<std::size_t>(n - (k + 1))), [=](sycl_ns::id<1> idx) {
                    const int i = k + 1 + static_cast<int>(idx[0]);
                    a[static_cast<std::size_t>(i) * n + k] = a[static_cast<std::size_t>(i) * n + k] / a[static_cast<std::size_t>(k) * n + k];
                });
            });
        }

        // Trailing matrix update: A(i,j) -= A(i,k)*A(k,j) for i>k, j>k
        if (k + 1 < n) {
            const std::size_t rows = static_cast<std::size_t>(n - (k + 1));
            const std::size_t cols = static_cast<std::size_t>(n - (k + 1));
            q.submit([&](sycl_ns::handler& h) {
                auto a = a_buf.template get_access<sycl_ns::access::mode::read_write>(h);
                h.parallel_for(sycl_ns::range<2>(rows, cols), [=](sycl_ns::id<2> id) {
                    const int i = k + 1 + static_cast<int>(id[0]);
                    const int j = k + 1 + static_cast<int>(id[1]);

                    const std::size_t ik = static_cast<std::size_t>(i) * n + k;
                    const std::size_t kj = static_cast<std::size_t>(k) * n + j;
                    const std::size_t ij = static_cast<std::size_t>(i) * n + j;
                    a[ij] = a[ij] - a[ik] * a[kj];
                });
            });
        }

        q.wait();
    }

    // Ensure data is copied back before returning.
    {
        auto host_acc = a_buf.template get_access<sycl_ns::access::mode::read>();
        (void)host_acc;
    }
}
