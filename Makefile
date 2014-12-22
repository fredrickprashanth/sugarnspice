
CC := gcc
MY_CFLAGS := -Wall -g -DNDEBUG
MY_DEPS := -lpthread -lrt -g

%.o: %.c
	$(CC) $(MY_CFLAGS) -c $< -o $@

my-progs := rt_task.bin ts_task.bin recv_nl_uevent.bin rt_hydra.bin fs_slammer.bin
my-progs += sched_test.bin malloc_test.bin cpu_hogd.bin frw.bin pipe_reader.bin pipe_writer.bin
my-progs += timerfd_test.bin test_pipe.bin test_spin.bin test_mmap.bin test_mmap2.bin
my-progs += test_waitpid.bin test_termios.bin test_free.bin test_env.bin test_file_del.bin
my-progs += test_nanosleep.bin test_pid_inode_leak.bin pci_config_rw.bin
my-progs += test_mmap3.bin test_xlock.bin test_sigabrt.bin test_clone.bin
my-progs += test_fork.bin test_fork_pthread.bin test_syswait.bin test_mysqrt.bin
my-progs += test_tasklock.bin test_pthread_waitpid.bin test_iorw.bin test_sigusr1.bin
my-progs += test_c_misc.bin test_c_misc2.bin cpukhz.bin test_mem_alerts.bin
my-progs += gnu_tsearch.bin my_strstr.bin producer_test.bin test_mask.bin test_select.bin
my-progs += run_cpu_affine.bin test_cache_alignment.bin test_bitfield.bin test_ncpus.bin
my-progs += test_sem.bin test_setcap.bin test_types.bin test_alignment.bin
my-progs += test_bitarray.bin test_bsearch.bin test_sparse_array.bin test_simple_pipe.bin
my-progs += test_large_mult.bin



.PHONY : all
all : $(my-progs)

%.bin: %.o
	$(CC) $(MY_DEPS) $< -o $@

test_xlock.bin: xlock.o 
test_xlock.bin: MY_DEPS += xlock.o

fs_slammer.bin: MY_DEPYS += -lpthread
pipe_writer.bin: utils.o
pipe_writer.bin: MY_DEPS += utils.o

test_sigabrt.bin: MY_DEPS += -pie
test_sigabrt.o: MY_CFLAGS += -fPIE

test_fork_pthread.bin: MY_DEPS += -lpthread
test_syswait.bin: MY_DEPS += -lpthread

test_tasklock.bin: MY_DEPS += -lpthread
test_pthread_waitpid.bin: MY_DEPS += -lpthread

test_setcap.bin: MY_DEPS += -lcap


test_sem.bin: xsem.o
test_sem.bin: xlock.o
test_sem.bin: MY_DEPS += xsem.o
test_sem.bin: MY_DEPS += xlock.o
	


.PHONY : clean
clean:
	rm -rf *.o
	rm -rf $(my-progs)
