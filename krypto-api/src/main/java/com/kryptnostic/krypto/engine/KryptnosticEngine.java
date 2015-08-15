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

    public KryptnosticEngine( String serverGlobalHash ) {
        handle = initKryptnosticEngine( serverGlobalHash.getBytes() );
    }

    public KryptnosticEngine( String privateKey, String publicKey, String serverGlobalHash ) {
        handle = initKryptnosticEngine( privateKey.getBytes(), publicKey.getBytes(), serverGlobalHash.getBytes() );
    }

    //
    //    Constructors
    //
    private native long initKryptnosticEngine( byte[] serverGlobalHash );
    private native long initKryptnosticEngine( byte[] privateKey, byte[] publicKey, byte[] serverGlobalHash );

    //
    //    Keys
    //
    public native String getPrivateKeys();
    public native String getPublicKeys();
    //    Factory with separate serialization
    public native String getServerSearchFunction();
    public native String getXor();
    public native String getAnd();
    public native String getShift();

    //
    //    Transformers
    //
    public native String mapTokenToKey( String word, String objectID );
    public native String getEncryptedSearchTerm( String word );
    public native String getBridgeKey( String docKey );

    //
    //    Generators
    //
    public native String genDocKey( String objectId );

    //
    //    Setters
    //
    public native String setDocKey( String objectId, String docKey );

}