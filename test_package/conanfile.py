import os
from conan import ConanFile
from conan.tools.build import can_run
from conan.tools.meson import MesonToolchain, Meson
from conan.tools.env import VirtualRunEnv

class RangTestConan(ConanFile):
    settings = "os", "compiler", "build_type", "arch"
    generators = "PkgConfigDeps", "VirtualRunEnv"

    def requirements(self):
        self.requires(self.tested_reference_str)

    def layout(self):
        from conan.tools.layout import basic_layout
        basic_layout(self)

    def generate(self):
        tc = MesonToolchain(self)
        tc.generate()

    def build(self):
        meson = Meson(self)
        meson.configure()
        meson.build()

    def test(self):
        if can_run(self):
            cmd = os.path.join(self.cpp.build.bindir, "visualTest")
            self.run(cmd, env="conanrun")
