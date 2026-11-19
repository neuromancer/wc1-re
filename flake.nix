{
  description = "Wing Commander source reconstruction and SDL2 port";

  inputs = {
    nixpkgs.url = "github:NixOS/nixpkgs/nixos-unstable";
    # Needs Nix >= 2.27; older Nix: nix build '.?submodules=1'
    self.submodules = true;
  };

  outputs = { self, nixpkgs }:
    let
      inherit (nixpkgs) lib;
      systems = [ "x86_64-linux" "aarch64-linux" ];
      forAllSystems = f: lib.genAttrs systems (system: f nixpkgs.legacyPackages.${system});

      # Sources the Slint launcher's CMake build normally downloads (Slint,
      # nfd, Corrosion via FetchContent, plus Slint's Rust crates). The Nix
      # sandbox has no network, so they are fetched once into a
      # fixed-output derivation.
      #
      # When the Slint/nfd versions pinned upstream change, set outputHash
      # back to lib.fakeHash and rebuild to get the new hash. A stale hash
      # silently keeps using the old sources.
      mkGuiDeps = pkgs: pkgs.stdenv.mkDerivation {
        name = "wc1-re-gui-deps";
        src = self;

        nativeBuildInputs = with pkgs; [ cmake ninja git cacert cargo rustc pkg-config python3 ];
        # Configure checks for these even though only sources are kept.
        buildInputs = with pkgs; [ SDL2 lzo dbus systemdLibs fontconfig ];
        dontUseCmakeConfigure = true;
        dontFixup = true; # fixup would add store references, not allowed here

        buildPhase = ''
          runHook preBuild
          export HOME=$TMPDIR CARGO_HOME=$TMPDIR/cargo-home
          cmake -S src/sdl/slint -B build -G Ninja -DCMAKE_BUILD_TYPE=Release
          ${lib.optionalString (builtins.pathExists ./nix/slint-Cargo.lock) ''
            cp ${./nix/slint-Cargo.lock} build/_deps/slint-src/Cargo.lock
          ''}
          cargo vendor --versioned-dirs \
            --manifest-path build/_deps/slint-src/Cargo.toml vendor > cargo-config.toml
          runHook postBuild
        '';

        installPhase = ''
          runHook preInstall
          mkdir -p $out/_deps
          cp -r build/_deps/*-src $out/_deps/
          find $out/_deps -name .git -prune -exec rm -rf {} +
          cp -r vendor $out/vendor
          sed 's|directory = "vendor"|directory = "@vendor@"|' cargo-config.toml > $out/cargo-config.toml
          runHook postInstall
        '';

        outputHashMode = "recursive";
        outputHashAlgo = "sha256";
        outputHash = "sha256-aYG+bBzlHyHEFjFUm1SXoS+mj1pLixQZviNz2Z4k1mo=";
      };

      mkWc1 = pkgs: { withGui ? false }:
        let
          guiDeps = mkGuiDeps pkgs;
          binary = if withGui then "wc1-modern-gui" else "wc1-modern";

          # Libraries loaded with dlopen at runtime: SDL3 (by sdl2-compat),
          # libGL (by SDL), and the windowing libraries winit uses for Slint.
          runtimeLibs = lib.makeLibraryPath ([ pkgs.sdl3 pkgs.libGL ]
            ++ lib.optionals withGui (with pkgs; [
              libxkbcommon wayland libx11 libxcursor libxi libxrandr
            ]));
        in
        pkgs.stdenv.mkDerivation {
          pname = binary;
          version = "0-unstable-${self.shortRev or "dirty"}";
          src = self;

          nativeBuildInputs = with pkgs; [ pkg-config python3 makeWrapper ]
            ++ lib.optionals withGui [ cmake ninja cargo rustc ];
          buildInputs = with pkgs; [ SDL2 lzo ]
            ++ lib.optionals withGui [ dbus systemdLibs fontconfig ];

          # Don't let the cmake/ninja setup hooks take over; the Makefile
          # drives everything.
          dontUseCmakeConfigure = true;
          dontUseNinjaCheck = true;

          preBuild = lib.optionalString withGui ''
            export HOME=$TMPDIR CARGO_HOME=$TMPDIR/cargo-home CARGO_NET_OFFLINE=true

            # Avoid the bare "s" in MAKEFLAGS reaching jemalloc's make via cargo.
            export CMAKE_GENERATOR=Ninja

            # Pre-populate the FetchContent sources and tell CMake not to fetch.
            # CMake reads CMAKE_TOOLCHAIN_FILE from the environment, which lets
            # us set the cache variable without changing the Makefile.
            mkdir -p out-modern/slint-gui/_deps
            cp -r --no-preserve=mode ${guiDeps}/_deps/. out-modern/slint-gui/_deps/
            echo 'set(FETCHCONTENT_FULLY_DISCONNECTED ON CACHE BOOL "" FORCE)' > $TMPDIR/offline.cmake
            export CMAKE_TOOLCHAIN_FILE=$TMPDIR/offline.cmake

            # Point cargo at the vendored crates.
            mkdir -p $CARGO_HOME
            sed 's|@vendor@|${guiDeps}/vendor|' ${guiDeps}/cargo-config.toml > $CARGO_HOME/config.toml
          '';

          # The Makefile sets CC to MSVC (via wibo) for the Win32 build;
          # override it for the native port.
          buildPhase = ''
            runHook preBuild
            make -j$NIX_BUILD_CORES ${if withGui then "modern-gui" else "modern"} CC=cc CXX=c++
            runHook postBuild
          '';

          installPhase = ''
            runHook preInstall
            install -Dm755 out-modern/${binary} $out/bin/${binary}
            wrapProgram $out/bin/${binary} \
              --prefix LD_LIBRARY_PATH : ${runtimeLibs} \
              --suffix LD_LIBRARY_PATH : /run/opengl-driver/lib
            runHook postInstall
          '';

          passthru = lib.optionalAttrs withGui { inherit guiDeps; };

          meta = {
            description = "Native SDL2 port of the reconstructed Wing Commander (1990)"
              + lib.optionalString withGui ", with the Slint launcher";
            homepage = "https://github.com/neuromancer/wc1-re";
            license = lib.licenses.gpl3Only; # TODO: confirm against LICENSE
            mainProgram = binary;
            platforms = lib.platforms.linux;
          };
        };
    in
    {
      packages = forAllSystems (pkgs: rec {
        wc1-modern = mkWc1 pkgs { };
        wc1-modern-gui = mkWc1 pkgs { withGui = true; };
        default = wc1-modern;
      });

      # Full environment for hacking on the port, including the Slint GUI.
      # Build the GUI with: make -j modern-gui CC=gcc CXX=g++
      devShells = forAllSystems (pkgs: {
        default = pkgs.mkShell {
          packages = with pkgs; [
            gcc gnumake pkg-config python3
            SDL2 sdl3 libGL lzo
            cmake ninja rustc cargo dbus systemdLibs fontconfig
          ];
          shellHook = ''
            export CMAKE_GENERATOR=Ninja
            export LD_LIBRARY_PATH=${lib.makeLibraryPath [ pkgs.sdl3 pkgs.libGL ]}:/run/opengl-driver/lib''${LD_LIBRARY_PATH:+:$LD_LIBRARY_PATH}
          '';
        };
      });
    };
}
