# speedtest-framework
Individual speed test framework especially for ciphers

## Bootstrap

### Setting up your test

There are 3 types of test for cipher.

- Generic: The generic tests are listed in `ALL_ALGORITHMS`. 
In this section, the framework will calculate the speed in Mbps(1000 bytes = 1 Kbytes).  
Your implementation MUST conform the struct `ALGORITHM`.  
```
typedef struct {
    char *name; // algorithm name
    char *description;
    unsigned int type;  // algorithm type
    int (*test_func)(void *ctx, uint8_t *text, uint32_t textlen, uint8_t *out); // algorithm test function
    void (*setup_func)(void **ctx); // setup functions like keygen
    void (*cleanup)(void **ctx); // cleanup function

} ALGORITHM;
```  
Note: You should use the `text` and the `textlen` in your test function implementation, due to the speed
is calculated with text size.

- Asymmetric: The asymmetric tests are listed in `ALL_ASYM_ALGORITHMS`.
In this section, the framework will calculate the speed in tps(times per second).  
Your implementation MUST conform the struct `ASYM_ALGORITHM`. Use the marco `ASYM_ALGORITHM_new` for convinience.
```
typedef struct {

    char *name; // algorithm names
    uint32_t keylen;
    unsigned int type;
    char *pub_description;
    char *pri_description;

    int (*pub_test_func)(void *ctx, uint8_t *text, uint32_t textlen, uint8_t *out); // algorithm test function
    void (*pub_setup_func)(void **ctx); // setup functions like keygen
    void (*pub_cleanup)(void **ctx); // cleanup function
    int (*pri_test_func)(void *ctx, uint8_t *text, uint32_t textlen, uint8_t *out); // algorithm test function
    void (*pri_setup_func)(void **ctx); // setup functions like keygen
    void (*pri_cleanup)(void **ctx); // cleanup function

} ASYM_ALGORITHM;
```  
And the marco defined as follows:  
```c
#define ASYM_ALGORITHM_new(name, NAME, keysize, type, pub_test_func, pri_test_func, setup_func, cleanup)
```  
Note: In this section, the textlen is the same size as the keylen in `ASYM_ALGORITHM`(for RSA compatiability), 
just ignore it if you would not use it, in this case you may use `text` as input with maximum length 16384(defined in speed.c)

- Single: The single tests are listed in `KEYGEN_ALGORITHMS`.
In this section, the framework will calculate the speed in tps(times per second).  
Your implementation MUST conform the struct `ALGORITHM`.  
Note: in single tests, the size of text (aka `textlen`) is ZERO. You may use `text` as input with maximum length 16384(defined in speed.c)

NOTE: The output space for test function as output has its maximum length 16384 + 1024(This should be enough for all ciphers)

### Demo

I've implemented aes tests from openssl, it requires openssl library.  
Modify the CMakeList.txt to track down your openssl lib.  
Remove aes_test.c/.h and the openssl dependency from cmakefile if you want to get rid of them.

### Command line

Command line args support and random generator support is from [c-tools](https://github.com/Tomahawkd/c-tools).  
For this framework, i've defined the following args:  
```
--help, -h                                Display this summary
--symmetric, -S                           Test symmetric ciphers
--asymmetric, -A                          Test asymmetric ciphers
--whitebox, -W                            Test whitebox ciphers
--hash, -H                                Test hash functions
--hmac, -M                                Test hmac functions
--keygen, -K                              Test keygen algorithms
--list, -l                                List avaliable crypto algorithms
--thread <thread num>, -t <thread num>    Set num of thread to run, set 0 for not use thread (Default: 1)
--seconds <interval>, -s <interval>       Set time interval for algorithm to loop (Default: 3)
```  
Although most of them are useless......

## Compatibility

For MacOS, the framework is fully compatiable.(My develop env)   
For Linux, need link to pthread and m(math lib).(Actually this should exist in Linux).  
For Windows, I've update an experimental speed test, but may still buggy.
