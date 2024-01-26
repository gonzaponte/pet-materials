{ self
, nixpkgs # <---- This `nixpkgs` has systems removed e.g. legacyPackages.zlib
, nain4
, ...
}: let
  inherit (nixpkgs.legacyPackages) pkgs;
  inherit (nain4.deps) args-from-cli make-app;
  in {

    packages.default = self.packages.pet-materials;

    # Executed by `nix run <URL of this flake>#pet-materials -- <args?>`
    # TODO: switch to clang environment
    packages.pet-materials = pkgs.stdenv.mkDerivation {
      pname = "pet-materials";
      version = "0.0.0";
      src = "${self}/src";
      nativeBuildInputs = [];
      buildInputs = [ nain4.packages.nain4 ];
    };

    # Used by `direnv` when entering this directory (also by `nix develop <URL to this flake>`)
    devShell = self.devShells.clang;

    # Activated by `nix develop <URL to this flake>#clang`
    devShells.clang = pkgs.mkShell.override { stdenv = nain4.packages.clang_16.stdenv; } {
      name = "pet-materials-clang-devenv";
      packages = nain4.deps.dev-shell-packages ++ [ nain4.packages.clang_16 ];
    };

    # Activated by `nix develop <URL to this flake>#gcc`
    devShells.gcc = pkgs.mkShell {
      name = "pet-materials-gcc-devenv";
      packages = nain4.deps.dev-shell-packages;
    };
}
