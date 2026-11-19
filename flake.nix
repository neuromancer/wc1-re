{
  description = "Wing Commander source reconstruction and SDL2 port";

  inputs = {
    nixpkgs.url = "github:NixOS/nixpkgs/nixos-unstable";
    # Needs Nix >= 2.27; older Nix: nix build '.?submodules=1'
    self.submodules = true;
  };

  outputs = { self, nixpkgs }:
    let
      systems = [ "x86_64-linux" "aarch64-linux" ];
      forAllSystems = f:
        nixpkgs.lib.genAttrs systems (system: f nixpkgs.legacyPackages.${system});

      # Loaded with dlopen at runtime: sdl2-compat loads SDL3, SDL loads libGL.
      runtimeLibs = pkgs: pkgs.lib.makeLibraryPath [ pkgs.sdl3 pkgs.libGL ];
    in
    {
      packages = forAllSystems (pkgs: rec {
        wc1-modern = pkgs.stdenv.mkDerivation {
          pname = "wc1-modern";
          version = "0-unstable-${self.shortRev or "dirty"}";
          src = self;

          nativeBuildInputs = with pkgs; [ pkg-config python3 makeWrapper ];
          buildInputs = with pkgs; [ SDL2 lzo ];

          # The Makefile sets CC to MSVC (via wibo) for the Win32 build;
          # override it for the native SDL2 port.
          buildPhase = ''
            runHook preBuild
            make -j$NIX_BUILD_CORES modern CC=cc CXX=c++
            runHook postBuild
          '';

          installPhase = ''
            runHook preInstall
            install -Dm755 out-modern/wc1-modern $out/bin/wc1-modern
            wrapProgram $out/bin/wc1-modern \
              --prefix LD_LIBRARY_PATH : ${runtimeLibs pkgs} \
              --suffix LD_LIBRARY_PATH : /run/opengl-driver/lib
            runHook postInstall
          '';

          meta = with pkgs.lib; {
            description = "Native SDL2 port of the reconstructed Wing Commander (1990)";
            homepage = "https://github.com/neuromancer/wc1-re";
            license = licenses.gpl3Only; # TODO: confirm against LICENSE
            mainProgram = "wc1-modern";
            platforms = platforms.linux;
          };
        };

        default = wc1-modern;
      });

      # Full environment for hacking on the port, including the Slint GUI.
      # Build the GUI with: make -j modern-gui CC=gcc CXX=g++
      devShells = forAllSystems (pkgs: {
        default = pkgs.mkShell {
          packages = with pkgs; [
            gcc gnumake pkg-config python3
            SDL2 sdl3 libGL lzo
            cmake ninja rustc cargo dbus fontconfig
          ];
          shellHook = ''
            # CMake's Makefile generator puts a bare "s" in MAKEFLAGS, which
            # breaks cargo -> jemalloc's make; Ninja avoids the nesting.
            export CMAKE_GENERATOR=Ninja
            export LD_LIBRARY_PATH=${runtimeLibs pkgs}:/run/opengl-driver/lib''${LD_LIBRARY_PATH:+:$LD_LIBRARY_PATH}
          '';
        };
      });
    };
}
