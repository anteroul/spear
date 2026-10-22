{
  description = "spear development environment";

  inputs.nixpkgs.url = "github:NixOS/nixpkgs/nixos-unstable";

  outputs =
    { nixpkgs, ... }:
    let
      systems = [
        "x86_64-linux"
        "aarch64-linux"
      ];
      forAllSystems = f: nixpkgs.lib.genAttrs systems (system: f nixpkgs.legacyPackages.${system});
    in
    {
      devShells = forAllSystems (pkgs: {
        default = pkgs.mkShell {
          nativeBuildInputs = with pkgs; [
            cmake
            ninja
            pkg-config
            clang-tools
            gdb
          ];

          buildInputs = with pkgs; [
            sdl3
            sdl3-image
            sdl3-ttf
            sdl3-mixer
            libGL
            glew
            vulkan-headers
            vulkan-loader
            vulkan-validation-layers
            bullet
            glm
            glslang
            spirv-tools
            shaderc
            shaderc.static
          ];

          VK_LAYER_PATH = "${pkgs.vulkan-validation-layers}/share/vulkan/explicit_layer.d";
          LD_LIBRARY_PATH = pkgs.lib.makeLibraryPath [
            pkgs.vulkan-loader
            pkgs.libGL
          ];
        };
      });
    };
}
