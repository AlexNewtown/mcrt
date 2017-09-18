config="debug"
all: FORCE
	premake5 gmake
	make -j8 -C build config=${config}
run: FORCE
	premake5 gmake
	make -j8 -C build mcrt config=${config}
	bin/mcrt ${args}
view-render: FORCE
	premake5 gmake
	make -j8 -C build mcrt config=${config}
	bin/mcrt render.png share/scene.json share/param.json
	feh --force-aliasing -F render.png

docs: FORCE
	make -C docs/
slides: FORCE
	make -C docs/slides/
tags: FORCE
	ctags -R src/mcrt include/mcrt
	cscope -Rb -s src/mcrt -s include/mcrt

clean: FORCE
	rm -rf build
	rm -rf docs/build
	rm -rf docs/slides/build
distclean: clean
	rm -rf bin
	rm -f docs/mcrt.pdf
	rm -f docs/slides/slides.pdf
FORCE:

# Clarifies gmake that these aren't any real dependencies...
.PHONY: all run view-render docs slides tags clean distclean
