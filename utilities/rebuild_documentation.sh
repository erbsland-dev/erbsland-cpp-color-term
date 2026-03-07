source .venv/bin/activate
rm -R _doxygen_input
rm -R _build
sphinx-build doc _build
