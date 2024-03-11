# 定义一个函数，强制重新定义源文件中的__FILE__宏为相对路径
function(force_redefine_file_macro_for_sources targetname)
    # 获取目标的所有源文件
    get_target_property(source_files "${targetname}" SOURCES)

    foreach(sourcefile ${source_files})
        # 获取源文件的绝对路径
        get_filename_component(filepath "${sourcefile}" ABSOLUTE)
        # 计算源文件相对于项目目录的路径
        string(REPLACE ${CMAKE_SOURCE_DIR}/ "" relpath ${filepath})
        
        # 生成重新定义__FILE__宏的字符串
        set(new_def "__FILE__=\"${relpath}\"")

        # 在源文件的编译定义中添加或替换__FILE__宏
        set_property(
            SOURCE "${sourcefile}"
            APPEND PROPERTY COMPILE_DEFINITIONS ${new_def}
        )
    endforeach()
endfunction()
