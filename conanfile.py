import os
from conan import ConanFile
from conan.tools.meson import MesonToolchain, Meson
from conan.tools.files import copy

class RangConan(ConanFile):
    name = "rang"
    version = "3.2.0"
    license = "The Unlicense"
    url = "https://github.com/agauniyal/rang"
    description = "A Minimal, Header only Modern c++ library for colors in your terminal"
    settings = "os", "compiler", "build_type", "arch"
    exports_sources = "meson.build", "include/*", "test/*", "CMakeLists.txt", "LICENSE", "README.md", "cmake/*"

    def requirements(self):
        self.test_requires("doctest/2.4.11")

    def build_requirements(self):
        self.tool_requires("pkgconf/2.1.0")

    def layout(self):
        from conan.tools.layout import basic_layout
        basic_layout(self)

    def generate(self):
        tc = MesonToolchain(self)
        tc.generate()
        from conan.tools.gnu import PkgConfigDeps
        pc = PkgConfigDeps(self)
        pc.generate()

    def build(self):
        meson = Meson(self)
        meson.configure()
        meson.build()

    def package(self):
        copy(self, "*.hpp", os.path.join(self.source_folder, "include"), os.path.join(self.package_folder, "include"))
        copy(self, "LICENSE", self.source_folder, os.path.join(self.package_folder, "licenses"))

    def package_id(self):
        self.info.clear()
