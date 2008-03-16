%define		date	20030704

Summary: 	An inventory manager and deck builder for "Vampire: The Eternal Struggle".
Name: 		wxARDB
Version: 	1.5.1
Release: 	cvs%{date}
Epoch: 		0
License: 	GPL
Group: 		Applications/Databases
URL: 		https://savannah.nongnu.org/projects/anarchdb/
Source0: 	%{name}-%{date}.tar.gz
#Source1:	ardb_icon.png
#Patch1:		wxARDB-configure-in-sqslite.patch
#Patch2:		wxARDB-src-Makefile-am-sqlite.patch
BuildRoot: 	%{_tmppath}/%{name}-%{version}-%{release}-buildroot
BuildRequires:	automake = 0:1.4 autoconf2.1
BuildRequires:  pkgconfig desktop-file-utils
BuildRequires:	sqlite-devel
BuildRequires:	libwxgtk-devel
BuildRequires:	libxml2-devel libxslt-devel
BuildRequires:	ImageMagick

# -----------------------------------------------------------------------------

%description
An inventory manager and deck builder for players of the collectible
card game "Vampire: The Eternal Struggle". 
It's first goal is to be a useful replacement for the Elder Library
Deck Builder, the currently used software, which is unfortunately
non-free, Windows-only, and unmaintained.

Inventory functions include: keeping track of your VTES collection
(have & need), taking notes and easy card browsing using various
selection criteria. Also handles inventory import/export.

Deck builder functions include: selection of cards to include,
automatic calculation of various statistics (crypt capacity, totals by
card type, Happy Families formula, etc.), simulated draw of cards, and
of course, saving and exporting to various formats (text, HTML, JOL).

# -----------------------------------------------------------------------------

%prep
%setup -q -n %{name}
#%patch1 -p0
#%patch2 -p0
#aclocal
#automake --add-missing
#autoconf
# CXXFLAGS="$RPM_OPT_FLAGS \"-DORIGINAL_DB=\\\"%{_datadir}/ardb/cards.db\\\"\"" %configure
#%confiure
#make %{?_smp_mflags}

convert src/icon.xpm ardb_icon.png
./make_linux.sh

# -----------------------------------------------------------------------------

%install
rm -rf $RPM_BUILD_ROOT
%makeinstall

cat > %{name}.desktop <<EOF
[Desktop Entry]
Name=%{name}
Comment=An inventory manager and deck builder for "Vampire: The Eternal Struggle".
Exec=ardb
Terminal=false
Icon=ardb_icon.png
Type=Application
Encoding=UTF-8
EOF

%{__mkdir_p} $RPM_BUILD_ROOT%{_datadir}/applications
desktop-file-install --vendor fedora            \
    --dir $RPM_BUILD_ROOT%{_datadir}/applications \
    --add-category X-Fedora                     \
    --add-category Application                  \
    --add-category CardGame			\
    --add-category Game				\
    %{name}.desktop

%{__mkdir_p} $RPM_BUILD_ROOT%{_datadir}/ardb

%{__mkdir_p} $RPM_BUILD_ROOT%{_datadir}/pixmaps
%{__install} -m 0644 ardb_icon.png $RPM_BUILD_ROOT%{_datadir}/pixmaps

# -----------------------------------------------------------------------------

%clean
rm -rf $RPM_BUILD_ROOT

# -----------------------------------------------------------------------------

%files
%defattr(-,root,root,-)
%doc AUTHORS COPYING INSTALL NEWS README
%{_bindir}/ardb
%{_datadir}/applications/*%{name}.desktop
%dir %{_datadir}/ardb
%{_datadir}/pixmaps/ardb_icon.png

# -----------------------------------------------------------------------------

%changelog
* Sun Jul  6 2003 Dams <anvil[AT]livna.org> 0:0.0.1-0.fdr.0.2.cvs20030704
- Package now own _datadir/ardb directory

* Wed May 21 2003 Dams <anvil[AT]livna.org> 
- Initial build.
