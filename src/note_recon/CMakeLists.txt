add_executable(note_recon AudioFile.cpp qtuner.cpp main.cpp qtuner.ui)
target_link_libraries(note_recon Qt5::Core Qt5::Gui Qt5::Multimedia Qt5::Widgets -lfftw3)