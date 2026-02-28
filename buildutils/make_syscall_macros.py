from sys import argv

out_file = ""
in_files = []


def get_files():
    global out_file
    out_file = argv[1]
    for i in range(2, len(argv)):
        in_files.append(argv[i])


def get_hex_no(x):
    return "0x" + hex(x)[2:].upper()


def parse_file(fname):
    file = open(fname)
    base = 0
    count = 0
    syscalls_started = False
    calls_dict = dict()
    for line in file:
        if line == "\n":
            continue
        if line.startswith("#define SYSCALL_BASE"):
            base = int(line[20:], 16)
            calls_dict["base"] = base
            continue
        if line.endswith("{\n"):
            syscalls_started = True
            continue
        if line.endswith("}\n"):
            break
        if syscalls_started and not line.strip().startswith("///") and line.strip().startswith("declare_syscall"):
            syscall_start = line.find("(") + 1
            syscall_end = line.find(")")
            syscall_name = line[syscall_start:syscall_end]
            syscall_no = base | count
            calls_dict["syscall_" + syscall_name] = get_hex_no(syscall_no)
            count += 1
    if not calls_dict["base"]:
        calls_dict["base"] = 0
    file.close()
    return calls_dict


def parse_all_files():
    parsed_file_data = []
    for file in in_files:
        calls_dict = parse_file(file)
        parsed_file_data.append(calls_dict)
    return parsed_file_data


def sort_files_data(parsed_file_data):
    return sorted(parsed_file_data, key=lambda x: x["base"])


def generate_tuples(sorted_file_data):
    files_syscalls = []
    for file_data in sorted_file_data:
        file_syscalls = []
        for k,v in file_data.items():
            if k == "base":
                continue
            file_syscalls.append((k, v))
        file_syscalls = sorted(file_syscalls, key=lambda x: int(x[1], 16))
        files_syscalls.append(file_syscalls)

    files_syscalls = sorted(files_syscalls, key=lambda x: int(x[0][1], 16))

    syscalls = []
    for file_syscalls in files_syscalls:
        syscalls.extend(file_syscalls)
        syscalls.append(("", ""))

    return syscalls


def get_max_tab(syscalls):
    maxlen = 0
    for call, _ in syscalls:
        maxlen = max(maxlen, len("#define " + call))

    if (maxlen & 3) == 0:
        maxlen += 4

    return ((maxlen + 3) >> 2) << 2


def generate_macros(syscalls, tabs):
    total_macros = "#ifndef _SYSCALL_MACROS_INCL\n#define _SYSCALL_MACROS_INCL\n\n"
    for call, value in syscalls:
        if call == "":
            total_macros += "\n"
            continue

        macro_string = "#define " + call
        macro_string += " " * (tabs - len(macro_string))
        macro_string += value
        macro_string += "\n"

        total_macros += macro_string

    total_macros += "#endif"
    return total_macros


def write_file(macros_str):
    file = open(out_file, "w")
    file.write(macros_str)


get_files()
parsed_file_data = parse_all_files()
sorted_file_data = sort_files_data(parsed_file_data)
syscalls = generate_tuples(sorted_file_data)
tab = get_max_tab(syscalls)
macros = generate_macros(syscalls, tab)
write_file(macros)
