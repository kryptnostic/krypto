package com.kryptnostic.krypto.engine;

import java.io.IOException;
import java.io.InputStream;
import java.nio.file.Files;
import java.nio.file.Path;
import java.nio.file.Paths;
import java.nio.file.StandardCopyOption;

/**
 * Anything that tests this must run with a larger than default stack size using the following VM argument:
 * -Xss7m
 * We've found 7 megs to be the smallest effective size
 *
 * @author Drew Bailey
 *
 */
public class KryptnosticEngine {
    public static final String PRIVATE_KEY                 = "KryptnosticEngine.PrivateKey";
    public static final String SEARCH_PRIVATE_KEY          = "KryptnosticEngine.SearchPrivateKey";
    public static final String CLIENT_HASH_FUNCTION        = "KryptnosticEngine.ClientHashFunction";
    public static final int    CLIENT_HASH_FUNCTION_LENGTH = 0;
    public static final int    INDEX_PAIR_LENGTH           = 2064;
    public static final int    SHARE_PAIR_LENGTH           = 2064;
    public static final int    SEARCH_PAIR_LENGTH          = 2080;

    static {
        final String osName = System.getProperty( "os.name" ).toLowerCase();
        final String libraryName = "libmain";
        String extension = ".so";
        if ( !osName.contains( "linux" ) ) {
            extension = ".dylib";
        }
        InputStream binary = KryptnosticEngine.class.getClassLoader().getResourceAsStream( libraryName + extension );
        final Path outputPath = Paths.get( System.getProperty( "java.io.tmpdir" ), "kryptnostic", libraryName
                + extension );
        try {
            Files.createDirectories( Paths.get( System.getProperty( "java.io.tmpdir" ), "kryptnostic" ) );
            Files.copy( binary, outputPath, StandardCopyOption.REPLACE_EXISTING );
        } catch ( IOException e1 ) {
            System.out.println( "Failed to copy file from resource storage to " + outputPath );
            System.out.println( e1.getStackTrace() );
        }
        System.load( outputPath.toAbsolutePath().toString() );
    }

    private long               kryptnosticServerPointer;

    protected long getKryptServer() {
        return kryptnosticServerPointer;
    }

    private long kryptnosticClientPointer;

    /**
     * VALIDATE HERE OR BEFORE PASSING IN
     *
     * @param clientHashFunction
     * @param encObjectSearchToken
     */
    public KryptnosticEngine( byte[] clientHashFunction, byte[] encObjectSearchToken ) {
        // Side effect, sets handle to a pointer to this object on the heap so we can clean it up
        initKryptnosticService( clientHashFunction, encObjectSearchToken );
    }

    /**
     * NEVER DO THIS
     */
    public KryptnosticEngine() {}

    @Override
    protected void finalize() throws Throwable {
        if ( kryptnosticClientPointer != 0 ) {
            destroyClient();
        }
        if ( kryptnosticServerPointer != 0 ) {
            destroyServer();
        }
        super.finalize();
    }

    /**
     * DONT
     */
    native void destroyServer();

    /**
     * DONT
     */
    native void destroyClient();

    /**
     * Constructor Constructs a KryptnosticServer given a client's hash function and an FHE-encrypted search token.
     * Calculates the client's hash function evaluated on the search token without the encrypted ObjectSearchKey
     **/
    private native void initKryptnosticService( byte[] clientHashFunction, byte[] encObjectSearchToken );

    /**
     * Function: calculateMetadataAddress Returns the client-computed metadata address from an ObjectIndexPair
     **/
    public native byte[] calculateMetadataAddress( byte[] objectIndexPair );

    /**
     * NEVER CALL THIS FROM THE SERVER Function: Client.initClient
     **/
    public native void initClient();

    /**
     * NEVER CALL THIS FROM THE SERVER DONT CALL THIS AND INITCLIENT() IN THE SAME JVM Function: Client.initClient
     **/
    public native void initClient( byte[] privateKey, byte[] searchPrivateKey );

    /**
     * NEVER CALL THIS FROM THE SERVER
     **/
    public native byte[] getSearchPrivateKey();

    /**
     * NEVER CALL THIS FROM THE SERVER
     **/
    public native byte[] getPrivateKey();

    /**
     * NEVER CALL THIS FROM THE SERVER
     **/
    /*
     * Function: getClientHashFunction() Returns a serialized concatenation of the three components of the
     * ClientHashFunction
     */
    public native byte[] getClientHashFunction();

    /**
     * NEVER CALL THIS FROM THE SERVER
     **/
    /*
     * Function: getObjectIndexPair() Returns a serialized ObjectSearchKey and ObjectAddressMatrix
     */
    public native byte[] getObjectIndexPair();

    /**
     * NEVER CALL THIS FROM THE SERVER
     **/
    /*
     * Function: getObjectSearchPairFromObjectIndexPair(objectIndexPair) Returns a serialized pair of (FHE-encrypted
     * ObjectSearchKey, ObjectAddressMatrix)
     */
    public native byte[] getObjectSearchPairFromObjectIndexPair( byte[] objectIndexPair );

    /**
     * NEVER CALL THIS FROM THE SERVER
     **/
    /*
     * Function: getObjectSearchPairFromObjectSharePair(objectSearchPair) Returns a serialized pair of (FHE-encrypted
     * objectSearchKey, objectConversionMatrix) Sent by a client to another to share a document
     */
    public native byte[] getObjectSharePairFromObjectSearchPair( byte[] objectSearchPair );

    /**
     * NEVER CALL THIS FROM THE SERVER
     **/
    /*
     * Function: getObjectSearchPairFromObjectSharePair(objectSharePair) Returns a serialized pair of (FHE-encrypted
     * objectSearchKey, objectConversionMatrix) Performed after the client receives a SharePair from another client
     * Assume the two inputs are AES-decrypted before passing in to C++
     */
    public native byte[] getObjectSearchPairFromObjectSharePair( byte[] objectSharePair );

    /**
     * NEVER CALL THIS FROM THE SERVER
     **/
    public native byte[] getEncryptedSearchToken( byte[] token );

    /**
     * NEVER CALL THIS FROM THE SERVER Function: Client.clientGetMetadatumAddress
     **/
    public native byte[] clientGetMetadatumAddress( byte[] objectIndexPair, byte[] token );

    protected static native byte[] testBitMatrixConversion( byte[] bytes );

    protected static native byte[] testBitVectorConversion( byte[] bytes );

    protected static native byte[] testClientHashFunctionConversion( byte[] bytes );

    protected static native byte[] testPrivateKey();

    protected static native byte[] testSearchPrivateKey();

    protected static native byte[] testObjectIndexPair( byte[] spk );

    protected static native byte[] testObjectSearchPairFromIndexPair( byte[] spk, byte[] oip, byte[] pk );

    protected static native byte[] testClientMetadataAddress( byte[] spk, byte[] oi, byte[] token );

    protected static native byte[] testClientHashFunction( byte[] spk, byte[] pk );

    protected static native byte[] testEncryptionFHE( byte[] pk, byte[] v );

}