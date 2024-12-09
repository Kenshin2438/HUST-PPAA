## HUST - Parallel Processing and Architecture 2024 Fall

---

### Lab: Memory Bandwidth Testing

1. C-code `memcpy`
2. ASM `sse2`

```
$ nice -n 0 ./build/membw
./build/membw: benchmark for memory bandwidth

================================================================================
== Memory bandwidth tests                                                     ==
== Note: If sample standard deviation exceeds 0.1%, it's shown in brackets.   ==
================================================================================

 C copy                                               :  17308.3 MB/s (1.7%)
 C copy backwards                                     :  17300.0 MB/s (0.4%)
 C copy prefetched                                    :  11863.8 MB/s
 C fill                                               :  12506.4 MB/s (0.4%)
 C memcpy()                                           :  17303.2 MB/s (0.2%)
 SSE2 copy                                            :  12734.7 MB/s (0.4%)
 SSE2 nontemporal copy                                :  17313.3 MB/s (0.5%)
 SSE2 nontemporal copy prefetched                     :  17361.8 MB/s (0.5%)
 SSE2 nontemporal fill                                :  17369.3 MB/s (0.5%)
```

3. Multi-threading `POSIX pthread` and `sched_setaffinity()`.

```
$ lscpu
Architecture:                    x86_64
CPU op-mode(s):                  32-bit, 64-bit
Byte Order:                      Little Endian
Address sizes:                   46 bits physical, 48 bits virtual
CPU(s):                          56
On-line CPU(s) list:             0-55
Thread(s) per core:              2
Core(s) per socket:              14
Socket(s):                       2
NUMA node(s):                    2
Vendor ID:                       GenuineIntel
CPU family:                      6
Model:                           63
Model name:                      Intel(R) Xeon(R) CPU E5-2683 v3 @ 2.00GHz
Stepping:                        2
CPU MHz:                         1200.587
CPU max MHz:                     3000.0000
CPU min MHz:                     1200.0000
BogoMIPS:                        4000.06
Virtualization:                  VT-x
L1d cache:                       896 KiB
L1i cache:                       896 KiB
L2 cache:                        7 MiB
L3 cache:                        70 MiB
NUMA node0 CPU(s):               0-13,28-41
NUMA node1 CPU(s):               14-27,42-55
Vulnerability Meltdown:          Mitigation; PTI
Vulnerability Spec store bypass: Vulnerable
Vulnerability Spectre v1:        Mitigation; __user pointer sanitization
Vulnerability Spectre v2:        Mitigation; Full generic retpoline
Flags:                           fpu vme de pse tsc msr pae mce cx8 apic sep mtrr pge mca cmov pat pse36 clflush dts acpi mmx fxsr sse sse2 ss ht tm pbe syscall nx pdpe1gb rdtscp lm constant_tsc arch_perfmon pebs bts rep
                                 _good nopl xtopology nonstop_tsc cpuid aperfmperf pni pclmulqdq dtes64 monitor ds_cpl vmx smx est tm2 ssse3 sdbg fma cx16 xtpr pdcm pcid dca sse4_1 sse4_2 x2apic movbe popcnt aes xsave av
                                 x f16c rdrand lahf_lm abm cpuid_fault epb invpcid_single pti intel_ppin tpr_shadow vnmi flexpriority ept vpid fsgsbase tsc_adjust bmi1 avx2 smep bmi2 erms invpcid cqm xsaveopt cqm_llc cqm
                                 _occup_llc dtherm ida arat pln pts
```

```
coder@f094ace882a5:~/project/membw$ nice -n 0 ./build/pmbw 1
pmbw (1 threads) :  16994.5 MB/s (3.7%)
coder@f094ace882a5:~/project/membw$ nice -n 0 ./build/pmbw 2
pmbw (2 threads) :  33716.6 MB/s (3.5%)
coder@f094ace882a5:~/project/membw$ nice -n 0 ./build/pmbw 3
pmbw (3 threads) :  37983.4 MB/s (3.6%)
coder@f094ace882a5:~/project/membw$ nice -n 0 ./build/pmbw 4
pmbw (4 threads) :  42110.0 MB/s (2.9%)
coder@f094ace882a5:~/project/membw$ nice -n 0 ./build/pmbw 5
pmbw (5 threads) :  41202.8 MB/s (2.7%)
coder@f094ace882a5:~/project/membw$ nice -n 0 ./build/pmbw 6
pmbw (6 threads) :  42063.8 MB/s (3.2%)
coder@f094ace882a5:~/project/membw$ nice -n 0 ./build/pmbw 7
pmbw (7 threads) :  41034.9 MB/s (3.8%)
coder@f094ace882a5:~/project/membw$ nice -n 0 ./build/pmbw 8
pmbw (8 threads) :  41727.5 MB/s (4.5%)
```

### Programming Assignment 1

1. Find all primes between $1$ and $10^{8}$
2. The Dining Philosophers problem

### Programming Assignment 2

Design and Implementation of Concurrent Queues

### [*Optional*] Programming Assignment 3

Checking Correctness of a Lock-Free Linked List