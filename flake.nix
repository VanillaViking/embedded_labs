{
  description = "A very basic flake";

  inputs = {
    nixpkgs.url = "github:nixos/nixpkgs?ref=nixos-unstable";
  };

  outputs = { self, nixpkgs }: {

    packages.x86_64-linux.hello = nixpkgs.legacyPackages.x86_64-linux.hello;

    let pkgs = nixpkgs.legacyPackages.x86_64-linux; in {
      devShells.default = import ./crossShell.nix { inherit pkgs; };
    }

  };
}
