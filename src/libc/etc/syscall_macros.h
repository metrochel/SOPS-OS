#ifndef _SYSCALL_MACROS_INCL
#define _SYSCALL_MACROS_INCL

#define syscall_exit                        0x0

#define syscall_put_character               0x1000
#define syscall_clear_screen                0x1001
#define syscall_print                       0x1002
#define syscall_positioned_print            0x1003
#define syscall_coloured_print              0x1004
#define syscall_positioned_coloured_print   0x1005
#define syscall_set_screen_bounds           0x1006
#define syscall_get_screen_size             0x1007
#define syscall_enable_cursor               0x1008
#define syscall_disable_cursor              0x1009

#define syscall_read_key                    0x2000
#define syscall_read_str                    0x2001

#define syscall_open_file                   0x3000
#define syscall_close_file                  0x3001
#define syscall_read                        0x3002
#define syscall_read_char                   0x3003
#define syscall_write                       0x3004
#define syscall_write_char                  0x3005
#define syscall_get_file_size               0x3006
#define syscall_remove_file                 0x3007
#define syscall_move_file                   0x3008

#define syscall_malloc                      0x4000
#define syscall_free                        0x4001
#define syscall_shell_cmd                   0x4002
#define syscall_get_env_var                 0x4003
#define syscall_set_env_var                 0x4004
#define syscall_get_unix_time               0x4005
#define syscall_get_nanosec_time            0x4006
#define syscall_get_proc_time               0x4007

#define syscall_start_thread                0x5000
#define syscall_exit_thread                 0x5001
#define syscall_get_current_thread          0x5002
#define syscall_yield                       0x5003
#define syscall_join_thread                 0x5004
#define syscall_block_thread                0x5005
#define syscall_timed_block_thread          0x5006
#define syscall_detach_thread               0x5007
#define syscall_create_mutex                0x5008
#define syscall_create_cond_var             0x5009
#define syscall_create_tss                  0x500A
#define syscall_destroy_mutex               0x500B
#define syscall_destroy_cond_var            0x500C
#define syscall_destroy_tss                 0x500D
#define syscall_get_mutex_locked            0x500E
#define syscall_lock_mutex                  0x500F
#define syscall_unlock_mutex                0x5010
#define syscall_signal_cond_var             0x5011
#define syscall_broadcast_cond_var          0x5012
#define syscall_get_tss                     0x5013
#define syscall_set_tss                     0x5014

#endif