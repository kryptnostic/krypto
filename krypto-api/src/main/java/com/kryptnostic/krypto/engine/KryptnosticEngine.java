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
        final Path outputPath = Paths.get( System.getProperty( "java.io.tmpdir" ), "kryptnostic", "fhe" + extension );
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

    public KryptnosticEngine( byte[] serverGlobalHash ) {
        handle = initKryptnosticEngine( serverGlobalHash );
    }

    public KryptnosticEngine( byte[] privateKey, byte[] publicKey, byte[] serverGlobalHash ) {
        handle = initKryptnosticEngine( privateKey, publicKey, serverGlobalHash );
    }

    public KryptnosticEngine() {
        handle = initKryptnosticEngine();
    }
    //
    //    Constructors
    //
    private native long initKryptnosticEngine();
    private native long initKryptnosticEngine( byte[] serverGlobalHash );
    private native long initKryptnosticEngine( byte[] privateKey, byte[] publicKey, byte[] serverGlobalHash );

    //
    //    Keys
    //
    public native byte[] getPrivateKeys();
    public native byte[] getPublicKeys();
    //    Factory with separate serialization
    public native byte[] getServerSearchFunction();
    public native byte[] getXor();
    public native byte[] getAnd();
    public native byte[] getShift();

    //
    //    Transformers
    //
    public native byte[] mapTokenToKey( byte[] word, byte[] objectID );
    public native byte[] getEncryptedSearchTerm( byte[] word );
    public native byte[] getBridgeKey( byte[] docKey );

    //
    //    Generators
    //
    public native byte[] genDocKey( byte[] objectId );

    //
    //    Setters
    //
    public native byte[] setDocKey( byte[] objectId, byte[] docKey );

}