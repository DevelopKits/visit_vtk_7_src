function bv_mesa_initialize
{
export DO_MESA="no"
export ON_MESA="off"
}

function bv_mesa_enable
{
DO_MESA="yes"
ON_MESA="on"
}

function bv_mesa_disable
{
DO_MESA="no"
ON_MESA="off"
}

function bv_mesa_depends_on
{
echo ""
}

function bv_mesa_info
{
export MESA_FILE=${MESA_FILE:-"mesa-11.2.2.tar.gz"}
export MESA_VERSION=${MESA_VERSION:-"11.2.2"}
export MESA_BUILD_DIR=${MESA_BUILD_DIR:-"mesa-11.2.2"}
export MESA_URL=${MESA_URL:-"https://mesa.freedesktop.org/archive/11.2.2/mesa-11.2.2.tar.gz"}
export MESA_MD5_CHECKSUM=""
export MESA_SHA256_CHECKSUM=""
}

function bv_mesa_print
{
printf "%s%s\n" "MESA_FILE=" "${MESA_FILE}"
printf "%s%s\n" "MESA_VERSION=" "${MESA_VERSION}"
printf "%s%s\n" "MESA_TARGET=" "${MESA_TARGET}"
printf "%s%s\n" "MESA_BUILD_DIR=" "${MESA_BUILD_DIR}"
}

function bv_mesa_print_usage
{
printf "%-15s %s [%s]\n" "--mesa" "Build Mesa" "$DO_MESA"
}

function bv_mesa_graphical
{
local graphical_out="Mesa     $MESA_VERSION($MESA_FILE)      $ON_MESA"
echo $graphical_out
}

function bv_mesa_host_profile
{
    if [[ "$DO_MESA" == "yes" ]] ; then
        echo >> $HOSTCONF
        echo "##" >> $HOSTCONF
        echo "## Mesa" >> $HOSTCONF
        echo "##" >> $HOSTCONF
        echo "VISIT_OPTION_DEFAULT(VISIT_MESA_DIR \${VISITHOME}/mesa/$MESA_VERSION/\${VISITARCH})" >> $HOSTCONF
    fi
}

function bv_mesa_selected
{
    args=$@
    if [[ $args == "--mesa" ]]; then
        DO_MESA="yes"
        ON_MESA="on"
        return 1
    fi

    return 0
}

function bv_mesa_ensure
{
    if [[ "$DO_DBIO_ONLY" != "yes" ]]; then
        if [[ "$DO_MESA" == "yes" ]] ; then
            ensure_built_or_ready "mesa"   $MESA_VERSION   $MESA_BUILD_DIR   $MESA_FILE
            if [[ $? != 0 ]] ; then
                return 1
            fi
        fi
    fi
}

function bv_mesa_dry_run
{
    if [[ "$DO_MESA" == "yes" ]] ; then
        echo "Dry run option not set for mesa."
    fi
}

function build_mesa
{
    #
    # prepare build dir
    #
    prepare_build_dir $MESA_BUILD_DIR $MESA_FILE
    untarred_mesa=$?

    if [[ $untarred_mesa == -1 ]] ; then
        warn "Unable to prepare Mesa build directory. Giving Up!"
        return 1
    fi

    #
    # Build Mesa.
    #
    info "Building Mesa . . . (~2 minutes)"
    cd $MESA_BUILD_DIR || error "Couldn't cd to mesa build dir."
    PF="${VISITDIR}/mesa/${MESA_VERSION}/${VISITARCH}"

    # We do the build twice due to a VTK issue.  VTK can establish a
    # rendering context via the system's GL using glX, via mangled Mesa
    # using glX, and via offscreen mangled Mesa.  For VisIt, we use
    # either the system's GL, or offscreen mangled Mesa.  To placate
    # VTK, we'll build a mangled+glX version, but then we'll build the
    # offscreen one that we really want.  This ensures we have the 'MesaGL'
    # that VTK needs to link, but if we use 'OSMesa' we get a real, OSMesa
    # library with no glX dependency.
    #
    # Due to this issue, it is imperative that one links "-lOSMesa
    # -lMesaGL" when they want to render/link to an offscreen Mesa
    # context.  The two libraries will have a host of duplicate
    # symbols, and it is important that we pick up the ones from OSMesa.
    info "Configuring Mesa (glX) ..."
    if [[ "$OPSYS" == "AIX" ]]; then
        export AIX_MESA_CFLAGS="-qcpluscmt -qlanglvl=extc99"
        autoconf
        if [[ $? != 0 ]] ; then
            error "Mesa: AIX autoconf failed!"
        fi
    fi

    if [[ "$DO_STATIC_BUILD" == "yes" ]]; then
        MESA_STATIC_DYNAMIC="--disable-shared --enable-static"
    fi

    info 
    
    # Neither of these should be necessary, but we use them as a temporary
    # workaround for a mesa issue.
    if test `uname` = "Linux" ; then
        HACK_FLAGS="-fPIC -DGLX_USE_TLS"
    fi

    # Do not build libGLU unless we're on MacOS X
    DISABLE_GLU="--disable-glu"
    if [[ "$OPSYS" == "Darwin" ]]; then
        DISABLE_GLU=""
        # If we're on 10.4 or earlier, change the GLU exports file
        VER=$(uname -r)
        if [[ ${VER%%.*} -le 9 ]]; then
            rm src/glu/sgi/glu.exports.darwin.edit
            sed "s/_\*/_m/g" src/glu/sgi/glu.exports.darwin > src/glu/sgi/glu.exports.darwin.edit
            cp src/glu/sgi/glu.exports.darwin.edit src/glu/sgi/glu.exports.darwin
        fi
    fi

    ./configure \
      CC="${C_COMPILER}" \
      CXX="${CXX_COMPILER}" \
      CFLAGS="${C_OPT_FLAGS} ${CFLAGS} ${AIX_MESA_CFLAGS} ${HACK_FLAGS}" \
      CXXFLAGS="${CXX_OPT_FLAGS} ${CXXFLAGS} ${HACK_FLAGS}" \
      --prefix=${PF}                    \
      --without-demos                   \
      --with-driver=osmesa              \
      --with-max-width=16384            \
      --with-max-height=16384           \
      --disable-glw                     \
      --disable-xvmc                    \
      --disable-glx                     \
      --disable-dri                     \
      --with-dri-drivers=               \
      --with-gallium-drivers=swrast     \
      --enable-texture-float            \
      --with-egl-platforms=             \
      --enable-gallium-osmesa           \
      --enable-gallium-llvm=yes         \
      ${DISABLE_GLU}                    \
      --disable-egl  ${MESA_STATIC_DYNAMIC}

    if [[ $? != 0 ]] ; then
        warn "Mesa: 'configure' for Offscreen failed.  Giving up"
        return 1
    fi

    info "Building Mesa (Offscreen) ..."
    ${MAKE} ${MAKE_OPT_FLAGS}
    if [[ $? != 0 ]] ; then
        warn "Mesa: 'make' for Offscreen failed.  Giving up"
        return 1
    fi
    info "Installing Mesa (Offscreen) ..."
    ${MAKE} install
    if [[ $? != 0 ]] ; then
        warn "Mesa: 'make install' for Offscreen failed.  Giving up"
        return 1
    fi

    # Some versions of Mesa erroneously install GLEW as well.  We need to make
    # sure we get VisIt's GLEW when we include it, so remove the ones Mesa
    # installs.
    rm -f ${PF}/include/GL/gl*ew.h

    if [[ $? != 0 ]] ; then
        warn "Mesa build failed.  Giving up"
        return 1
    fi

    if [[ "$DO_GROUP" == "yes" ]] ; then
        chmod -R ug+w,a+rX "$VISITDIR/mesa"
        chgrp -R ${GROUP} "$VISITDIR/mesa"
    fi
    cd "$START_DIR"
    info "Done with Mesa"
    return 0
}

function bv_mesa_is_enabled
{
    if [[ $DO_MESA == "yes" ]]; then
        return 1    
    fi
    return 0
}

function bv_mesa_is_installed
{
    check_if_installed "mesa" $MESA_VERSION
    if [[ $? == 0 ]] ; then
        return 1
    fi
    return 0
}

function bv_mesa_build
{
    #
    # Build Mesa
    #
    cd "$START_DIR"
    if [[ "$DO_MESA" == "yes" ]] ; then
        check_if_installed "mesa" $MESA_VERSION
        if [[ $? == 0 ]] ; then
            info "Skipping Mesa build.  Mesa is already installed."
        else
            info "Building Mesa (~2 minutes)"
            build_mesa
            if [[ $? != 0 ]] ; then
                error "Unable to build or install Mesa.  Bailing out."
            fi
            info "Done building Mesa"
        fi
    fi
}

