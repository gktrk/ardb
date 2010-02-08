
#SUBDIRS = resources

RESCOMP = @RESCOMP@
ARDB_WX_LIBS = @ARDB_WX_LIBS@
ARDB_XML2_LIBS = @XML2_LIBS@
ARDB_XSLT_LIBS = @XSLT_LIBS@

INCLUDES = $(SQLITE_CFLAGS)

if USE_WINDOWS
RESOURCE_FILES = ardb_resource.o
endif

bin_PROGRAMS = ardb

ardb_SOURCES = \
	amountdialog.cpp \
	browsercryptfilter.cpp \
	browsercryptcontroller.cpp \
	browsercryptmodel.cpp \
	browsercrypttab.cpp \
	browserlibraryfilter.cpp \
	browserlibrarycontroller.cpp \
	browserlibrarymodel.cpp \
	browserlibrarytab.cpp \
	cardtext.cpp \
	database.cpp \
	deckcrypttab.cpp \
	deckinfotab.cpp \
	decklibrarytab.cpp \
	deckmodel.cpp \
	deckwindow.cpp \
	drawsimulator.cpp \
	editionsdialog.cpp \
	interfacedata.cpp \
	inventorymodel.cpp \
	main.cpp \
	speling.cpp \
	types.cpp \
	updater.cpp \
	imagedialog.cpp \
	imagePanel.cpp \
	ardb_db_edition_filter.cpp \
	importxml.cpp \
	cardviewer.cpp \
	DeckUpload.cpp \
	sllogindialog.cpp \
	preferencedialog.cpp

ardb_resource.o: ardb.rc
	$(RESCOMP) -i$(srcdir)/ardb.rc -o ardb_resource.o \
        --include-dir $(srcdir)/../resources @RC_WX_INCLUDES@ \
        --define __WIN32__ --define __WIN95__ --define __GNUWIN32__

rez: $(bin_PROGRAMS)
	@AFTERBUILD@

all-local: $(bin_PROGRAMS) rez

ardb_LDADD = $(RESOURCE_FILES) $(ARDB_WX_LIBS) $(IMGLDFLAGS) $(SQLITE_LIBS) $(ARDB_XML2_LIBS) $(ARDB_XSLT_LIBS)
