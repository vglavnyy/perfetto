project(
    'issue_271', 'cpp',
    version: '0.1.0',
    default_options: [
        'c_std=c17',
        'cpp_std=c++17',
        'warning_level=3',
        'werror=true',
        'b_sanitize=address,undefined',
        'b_pie=true',
        'b_lundef=false',
        'perfetto:werror=false',
    ]
)

perfetto_dep = dependency('perfetto')

issue_271 = executable(
    'issue_271',
    sources: ['issue_271.cc'],
    cpp_args: ['-DISSUE_271'],
    dependencies: [perfetto_dep]
)
