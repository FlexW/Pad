
CC= clang
COBJECTS= test-driver-canvas.o \
	pad-point.o \
	pad-bounding-box.o \
	pad-canvas.o \
	pad-canvas-item.o \
	pad-canvas-item-group.o \
	pad-canvas-item-polyline.o \
	pad-canvas-item-pdf.o \
	pad-canvas-debug.o \
	pad-coordinate-system.o

PACKAGES= gtk+-3.0 poppler-glib
PKGCONFIG= pkg-config
CFLAGS:= $(shell $(PKGCONFIG) --cflags $(PACKAGES))
CFLAGS:= $(CFLAGS)
CLIBS:= $(shell $(PKGCONFIG) --libs $(PACKAGES))

.PHONY: all clean clean-obj

all: test_driver_canvas pad_canvas pad_canvas_debug pad_coordinate_system prog

prog: $(COBJECTS)
	$(CC) $(COBJECTS) $(CLIBS)
	$(RM) *.o *.so *.orig *~ core* *.hh.gch *.h.gch

test_driver_canvas: test-driver-canvas.c
	$(CC) test-driver-canvas.c -c $(CFLAGS)

pad_point: pad-point.c pad-point.h
	$(CC) pad-point.c -c $(CFLAGS)

pad_bounding_box: pad-bounding_box.c pad-bounding-box.h
	$(CC) pad-bounding-box.c -c $(CFLAGS)

pad_canvas: pad-canvas.c pad-canvas.h
	$(CC) pad-canvas.c -c $(CFLAGS)

pad_canvas_item: pad-canvas-item.c pad-canvas-item.h
	$(CC) pad-canvas-item.c -c $(CFLAGS)

pad_canvas_item_group: pad-canvas-item-group.c pad-canvas-item-group.h
	$(CC) pad-canvas-item-group.c -c $(CFLAGS)

pad_canvas_item_polyline: pad-canvas-item-polyline.c pad-canvas-item-polyline.h
	$(CC) pad-canvas-item-polyline.c -c $(CFLAGS)

pad_canvas_item_pdf: pad-canvas-item-pdf.c pad-canvas-item-pdf.h
	$(CC) pad-canvas-item-pdf.c -c $(CFLAGS)

pad_canvas_debug: pad-canvas-debug.c pad-canvas-debug.h
	$(CC) pad-canvas-debug.c -c $(CFLAGS)

pad_coordinate_system: pad-coordinate-system.c pad-coordinate-system.h
	$(CC) pad-coordinate-system.c -c $(CFLAGS)

clean:
	$(RM) *.o *.so *.orig *~ a.out core* *.hh.gch *.h.gch
