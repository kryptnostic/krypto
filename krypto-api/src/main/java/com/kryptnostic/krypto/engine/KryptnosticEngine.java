package com.kryptnostic.krypto.engine;

import java.io.IOException;
import java.io.InputStream;
import java.nio.file.Files;
import java.nio.file.Path;
import java.nio.file.Paths;
import java.nio.file.StandardCopyOption;

public class KryptnosticEngine {

    static {
        final String osName = System.getProperty( "os.name" ).toLowerCase();
        final String libraryName = "libmain";
        String extension = ".so";
        if ( !osName.contains( "linux" ) ){
            extension = ".dylib";
        }
        InputStream binary = KryptnosticEngine.class.getClassLoader().getResourceAsStream( libraryName + extension );
        final Path outputPath = Paths.get( System.getProperty( "java.io.tmpdir" ), "kryptnostic", libraryName + extension );
        try {
            Files.createDirectories( Paths.get( System.getProperty( "java.io.tmpdir" ), "kryptnostic") );
            Files.copy( binary , outputPath , StandardCopyOption.REPLACE_EXISTING );
        } catch ( IOException e1 ) {
            System.out.println( "Failed to copy file from resource storage to " + outputPath );
            System.out.println( e1.getStackTrace() );
        }
        System.load( outputPath.toAbsolutePath().toString() );
    }

    private final long handle;

    public KryptnosticEngine( byte[] clientHashFunction, byte[] conversionMatrix, byte[] docSearchKey ) {
        handle = initKryptnosticEngine( clientHashFunction, conversionMatrix, docSearchKey );
    }

    //
    //    Constructors
    //
    // ryan sends docSearchKey, convMatrix per document & clientHashFunc per session
    // make a set of Pair<ConvMatrix, docSearchKey>
    private native long initKryptnosticEngine( byte[] clientHashFunction, byte[] conversionMatrix, byte[] docSearchKey );

    // search => call this
    public native long getAddress( byte[] encSearchToken );

}