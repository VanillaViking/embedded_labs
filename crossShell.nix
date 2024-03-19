with import <nixpkgs> {
};

mkShell {
  buildInputs = [ pkgsCross.avr.buildPackages.gcc pkgsCross.avr.libcCross avrdude screen ]; # your dependencies here
}
