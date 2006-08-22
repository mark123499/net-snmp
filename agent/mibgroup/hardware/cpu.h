config_require(hardware/cpu/cpu)
config_arch_require(linux, hardware/cpu/cpu_linux)

config_arch_require(freebsd2, hardware/cpu/cpu_nlist)
config_arch_require(freebsd3, hardware/cpu/cpu_nlist)
config_arch_require(freebsd4, hardware/cpu/cpu_nlist)
config_arch_require(freebsd5, hardware/cpu/cpu_nlist)
config_arch_require(freebsd6, hardware/cpu/cpu_nlist)

config_arch_require(netbsd,     hardware/cpu/cpu_sysctl)
config_arch_require(netbsd1,    hardware/cpu/cpu_sysctl)
config_arch_require(netbsdelf,  hardware/cpu/cpu_sysctl)
config_arch_require(netbsdelf2, hardware/cpu/cpu_sysctl)
config_arch_require(netbsdelf3, hardware/cpu/cpu_sysctl)
config_arch_require(openbsd2,   hardware/cpu/cpu_sysctl)
config_arch_require(openbsd3,   hardware/cpu/cpu_sysctl)

config_arch_require(aix5, hardware/cpu/cpu_perfstat)
