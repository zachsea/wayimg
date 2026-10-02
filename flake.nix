{
  description = "Image viewer for Wayland focused on tiling window managers";

  inputs = {
    nixpkgs.url = "github:NixOS/nixpkgs/nixos-unstable";
    flake-utils.url = "github:numtide/flake-utils";
  };

  outputs =
    {
      self,
      nixpkgs,
      flake-utils,
    }:
    flake-utils.lib.eachDefaultSystem (
      system:
      let
        pkgs = import nixpkgs { inherit system; };
      in
      {
        devShells.default = pkgs.mkShell {
          nativeBuildInputs = with pkgs; [
            cmake
            pkg-config
            ninja
            gdb
          ];
          buildInputs = with pkgs; [
            sdl3
            sdl3-image
          ];
        };

        packages.default = pkgs.stdenv.mkDerivation {
          pname = "wayimg";
          version = "0.1.0";
          src = ./.;

          nativeBuildInputs = with pkgs; [
            cmake
            pkg-config
            ninja
          ];
          buildInputs = with pkgs; [
            sdl3
            sdl3-image
          ];
        };
      }
    )
    // {
      overlays.default = final: _prev: {
        wayimg = self.packages.${final.stdenv.hostPlatform.system}.default;
      };
    };
}
