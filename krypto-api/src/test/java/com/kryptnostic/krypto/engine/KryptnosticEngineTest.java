package com.kryptnostic.krypto.engine;

import java.nio.ByteBuffer;
import java.util.Random;

import org.junit.After;
import org.junit.AfterClass;
import org.junit.Assert;
import org.junit.Before;
import org.junit.BeforeClass;
import org.junit.Test;

import static org.junit.Assert.assertArrayEquals;

public class KryptnosticEngineTest {

    @BeforeClass
    public static void setUpBeforeClass() throws Exception {}

    @AfterClass
    public static void tearDownAfterClass() throws Exception {}

    @Before
    public void setUp() throws Exception {}

    @After
    public void tearDown() throws Exception {}
    
    @Test
    public void testBitVectorConversions() throws Exception {
        Random rand = new Random();
        byte[] vector = new byte[ 16 ];
        rand.nextBytes( vector );
        byte[] output = KryptnosticEngine.testBitVectorConversion( vector );
        assertArrayEquals( vector, output );
    }

    @Test
    public void testBitMatrixConversions() throws Exception {
        Random rand = new Random();
        byte[] matrix = new byte[ 2048 ];
        rand.nextBytes( matrix );
        byte[] output = KryptnosticEngine.testBitMatrixConversion( matrix );
        assertArrayEquals( matrix, output );
    }

    @Test
    public void testClientHashFunctionConversions() throws Exception {
        Random rand = new Random();
        byte[] chf = new byte[ 1060896 ];
        rand.nextBytes( chf );
        byte[] output = KryptnosticEngine.testClientHashFunctionConversion( chf );
        assertArrayEquals( chf, output );
    }

    @Test
    public void testShareFlow() {
        KryptnosticEngine clientEngine = new KryptnosticEngine();
        KryptnosticEngine clientEngine2 = new KryptnosticEngine();

        clientEngine.initClient();
        clientEngine2.initClient();

        Random r = new Random();

        byte[] searchToken = new byte[ 16 ];

        r.nextBytes( searchToken );

        byte[] encryptedSearchToken = clientEngine.getEncryptedSearchToken( searchToken );
        byte[] encryptedSearchToken2 = clientEngine2.getEncryptedSearchToken( searchToken );
        KryptnosticEngine serverEngine = new KryptnosticEngine(
                clientEngine.getClientHashFunction(),
                encryptedSearchToken );
        KryptnosticEngine serverEngine2 = new KryptnosticEngine(
                clientEngine2.getClientHashFunction(),
                encryptedSearchToken2 );
        
        byte[] objectIndexPair = clientEngine.getObjectIndexPair();

        byte[] address = clientEngine.clientGetMetadatumAddress( objectIndexPair, searchToken );

        byte[] objectSearchPair = clientEngine.getObjectSearchPairFromObjectIndexPair( objectIndexPair );

        // Tests sharing
        byte[] objectSharePair = clientEngine.getObjectSharePairFromObjectSearchPair( objectSearchPair );
        byte[] objectSearchPair2 = clientEngine2.getObjectSearchPairFromObjectSharePair( objectSharePair );

        byte[] serverAddressDirect = serverEngine.calculateMetadataAddress( objectSearchPair );
        byte[] serverAddressFromSharing = serverEngine2.calculateMetadataAddress( objectSearchPair2 );
        
        Assert.assertArrayEquals( address, serverAddressDirect );
        Assert.assertArrayEquals( address, serverAddressFromSharing );
        Assert.assertArrayEquals( serverAddressDirect, serverAddressFromSharing );
        clientEngine.destroyClient();
        clientEngine2.destroyClient();
        serverEngine.destroyServer();
        serverEngine2.destroyServer();
    }

    @Test
    public void testSearchFlow() throws Exception {
        Random rand = new Random();
        
        byte[] searchToken = new byte[ 16 ];
        rand.nextBytes( searchToken );
        
        byte[] pk = KryptnosticEngine.testPrivateKey();
        byte[] spk = KryptnosticEngine.testSearchPrivateKey();
        
        byte[] oip = KryptnosticEngine.testObjectIndexPair( spk );
        byte[] osk = KryptnosticEngine.testObjectSearchKey( spk );
        byte[] osp = KryptnosticEngine.testObjectSearchPairFromIndexPair( spk, oip, pk );
        byte[] token = new byte[ 16 ];
        rand.nextBytes( token );
        byte[] clientCalculatedAddress = KryptnosticEngine.testClientMetadataAddress( spk, oip, token );

        byte[] etoken = KryptnosticEngine.testEncryptionFHE( pk, token );
        byte[] eosk = KryptnosticEngine.testEncryptionFHE( pk, osk );
        byte[] chf = KryptnosticEngine.testClientHashFunction( spk, pk );

        KryptnosticEngine ke = new KryptnosticEngine( chf, etoken );
        byte[] serverCalculatedAddress = ke.calculateMetadataAddress( osp );

        // System.out.print("Client-calculated = [");
        // for( byte b : clientCalculatedAddress ){
        // System.out.print( (int) b );
        // System.out.print(",");
        // }
        // System.out.println("]");
        //
        // System.out.print("Server-calculated = [");
        // for( byte b : serverCalculatedAddress ){
        // System.out.print( (int) b );
        // System.out.print(",");
        // }
        // System.out.println("]");

        assertArrayEquals( clientCalculatedAddress, serverCalculatedAddress );
        ke.destroyServer();
    }
}
