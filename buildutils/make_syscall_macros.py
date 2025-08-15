from sys import argv


def get_fname() -> (str, str):
    return argv[1], argv[2]


def get_data(fname: str) -> list[str]:
    f = open(fname)
    data = f.readlines()
    f.close()
    return data


def parse_data(data: list[str]) -> list[tuple]:
    syscalls = []
    syscalls_started = False
    for line in data:
        if line.startswith('enum'):
            syscalls_started = True
            continue
        if line.startswith('}'):
            break
        if not syscalls_started: continue
        if not line or line == '\n' or line.strip().startswith('/'): continue
        name, value = line.split(' = ')
        value = value[:-1].strip()
        if value.endswith(','): value = value[:-1]
        name = name.strip()
        syscalls.append((name, value))
    return syscalls


def compute_tab(syscalls: list[tuple]) -> int:
    max_tab = 0
    for name, value in syscalls:
        strlen = len(name) + len("#define Syscall_")
        tab = strlen + 4 - (strlen % 4)
        if tab > max_tab:
            max_tab = tab
    return max_tab


def generate_macros(syscalls: list[tuple], tab: int) -> str:
    macros_str = ''
    for name, value in syscalls:
        length = len("#define Syscall_" + name)
        macro = '#define Syscall_' + name + ' ' * (tab - length) + value + '\n'
        macros_str += macro
    return macros_str


def write_macros_file(fname: str, macros: str):
    file = open(fname, 'w')
    file.write('#ifndef _SYSCALL_MACROS_INCL\n#define _SYSCALL_MACROS_INCL\n\n')
    file.write(macros)
    file.write('\n#endif')
    file.close()


in_fname, out_fname = get_fname()
data = get_data(in_fname)
syscalls = parse_data(data)
tab = compute_tab(syscalls)
macros = generate_macros(syscalls, tab)
write_macros_file(out_fname, macros)
