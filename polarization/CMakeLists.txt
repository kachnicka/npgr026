set(Polarization_files
    main.cpp
    Polarization.h)

add_executable(polarization ${Polarization_files})

target_compile_features(polarization PUBLIC cxx_std_17)
target_link_libraries(polarization PRIVATE glm::glm)