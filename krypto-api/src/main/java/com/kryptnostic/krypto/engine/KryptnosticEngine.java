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

    private long handle;

    protected long getHandle(){
        return handle;
    }

    /**
     * VALIDATE HERE OR BEFORE PASSING IN
     * @param clientHashFunction
     * @param encObjectSearchToken
     */
    public KryptnosticEngine( byte[] clientHashFunction, byte[] encObjectSearchToken ) {
        // Side effect, sets handle to a pointer to this object on the heap so we can clean it up
        initKryptnosticService( clientHashFunction, encObjectSearchToken );
    }

    /**
     * Constructor
     * Constructs a KryptnosticServer given a
     * client's hash function and an FHE-encrypted search token.
     * Calculates the client's hash function evaluated
     * on the search token without the encrypted ObjectSearchKey
     **/
    private native void initKryptnosticService( byte[] clientHashFunction, byte[] encObjectSearchToken );

    /**
     * Function: getMetadataAddress
     * Returns a serialized pair of (ObjectSearchKey, ObjectAddressFunction)
     **/
    public native byte[] calculateMetadataAddress( byte[] encObjectSearchKey, byte[] objectConversionMatrix );

    protected static native byte[] testBitMatrixConversion( byte[] bytes );
    protected static native byte[] testBitVectorConversion( byte[] bytes );
    protected static native byte[] testClientHashFunctionConversion( byte[] bytes );
    
    protected static native byte[] testPrivateKey();
    protected static native byte[] testSearchPrivateKey();
    protected static native byte[] testObjectSearchKey( byte[] spk );
    protected static native byte[] testObjectAddressMatrix( byte[] spk );
    protected static native byte[] testObjectConversionMatrix( byte[] spk, byte[] oam );
    protected static native byte[] testClientMetadataAddress( byte[] spk, byte[] oam, byte[] osk, byte[] token );
    protected static native byte[] testClientHashFunction( byte[] spk, byte[] pk );
    protected static native byte[] testEncryptionFHE( byte[] pk, byte[] v );

}