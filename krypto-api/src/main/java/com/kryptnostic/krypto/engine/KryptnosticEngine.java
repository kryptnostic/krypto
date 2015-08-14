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
        final String libraryName = "mainlib";
        String extension = "so";
        if ( !osName.contains( "linux" ) ){
            extension = "dylib";
        }
        final InputStream binaryAsStream = KryptnosticEngine.class.getResourceAsStream( libraryName + extension );
        final Path outputPath = Paths.get( "tmp", "kryptnostic", "fhe" + extension );
        try {
            Files.copy( binaryAsStream , outputPath , StandardCopyOption.REPLACE_EXISTING );
        } catch ( IOException e1 ) {
            System.out.println( "Failed to copy file from resource storage to " + outputPath );
            System.out.println( e1.getStackTrace() );
        }
        System.load( outputPath.toAbsolutePath().toString() );
    }

    public KryptnosticEngine() {
    }

}