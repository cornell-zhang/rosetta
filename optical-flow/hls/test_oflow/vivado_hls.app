<AutoPilot:project xmlns:AutoPilot="com.autoesl.autopilot.project" projectType="C/C++" name="test_oflow" top="optical_flow">
    <Simulation argv="-p /home/pbc48/rosetta_g/rosetta/optical-flow/datasets/sintel_alley -o out.flo">
        <SimFlow name="csim" setup="false" optimizeCompile="false" clean="false" ldflags="" mflags=""/>
    </Simulation>
    <includePaths/>
    <libraryFlag/>
    <files>
        <file name="../../imageLib/imageLib.h" sc="0" tb="1" cflags=" "/>
        <file name="../../imageLib/flowIO.h" sc="0" tb="1" cflags=" "/>
        <file name="../../../imageLib/flowIO.cpp" sc="0" tb="1" cflags=" "/>
        <file name="../../../imageLib/RefCntMem.h" sc="0" tb="1" cflags=" "/>
        <file name="../../../imageLib/RefCntMem.cpp" sc="0" tb="1" cflags=" "/>
        <file name="../../imageLib/ImageIO.h" sc="0" tb="1" cflags=" "/>
        <file name="../../../imageLib/ImageIO.cpp" sc="0" tb="1" cflags=" "/>
        <file name="../../imageLib/Image.h" sc="0" tb="1" cflags=" "/>
        <file name="../../../imageLib/Image.cpp" sc="0" tb="1" cflags=" "/>
        <file name="../../imageLib/Convolve.h" sc="0" tb="1" cflags=" "/>
        <file name="../../../imageLib/Convolve.cpp" sc="0" tb="1" cflags=" "/>
        <file name="../../../imageLib/Convert.cpp" sc="0" tb="1" cflags=" "/>
        <file name="../../host/typedefs.h" sc="0" tb="1" cflags=" "/>
        <file name="../../host/utils.h" sc="0" tb="1" cflags=" "/>
        <file name="../../host/check_result.h" sc="0" tb="1" cflags=" "/>
        <file name="../../host/utils.cpp" sc="0" tb="1" cflags=" "/>
        <file name="../../host/check_result.cpp" sc="0" tb="1" cflags=" "/>
        <file name="../../host/optical_flow_host.cpp" sc="0" tb="1" cflags=" "/>
        <file name="sdsoc/optical_flow.h" sc="0" tb="false" cflags=""/>
        <file name="sdsoc/optical_flow.cpp" sc="0" tb="false" cflags=""/>
    </files>
    <solutions>
        <solution name="solution1" status=""/>
    </solutions>
</AutoPilot:project>

