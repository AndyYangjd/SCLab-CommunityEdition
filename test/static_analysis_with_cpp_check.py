import subprocess
import sys

def main():
    cppcheck_command = "cppcheck --enable=all --xml --xml-version=2 ./"
    result = subprocess.run(cppcheck_command.split(), capture_output=True, text=True)

    # 在这里添加自定义规则
    def custom_filter(error):
        # 示例规则：过滤所有包含特定文件名的警告
        return "my_special_file.cpp" not in error

    # 添加更多规则
    def custom_filter_2(error):
        # 示例规则：过滤所有包含特定错误类型的警告
        return "error type" not in error

    # 将所有规则放入一个列表中
    filters = [custom_filter, custom_filter_2]

    # 对每个规则进行过滤
    filtered_errors = result.stderr.splitlines()
    for filter_func in filters:
        filtered_errors = list(filter(filter_func, filtered_errors))

    for error in filtered_errors:
        print(error)

if __name__ == "__main__":
    main()