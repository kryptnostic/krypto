package com.kryptnostic.krypto.engine;

import java.util.Arrays;
import java.util.Random;
import java.util.stream.Collectors;

import org.junit.After;
import org.junit.AfterClass;
import org.junit.Before;
import org.junit.BeforeClass;
import org.junit.Test;

import static org.junit.Assert.assertNotNull;
import static org.junit.Assert.assertArrayEquals;

public class KryptnosticEngineTest {

	@BeforeClass
	public static void setUpBeforeClass() throws Exception {
	}

	@AfterClass
	public static void tearDownAfterClass() throws Exception {
	}

	@Before
	public void setUp() throws Exception {
	}

	@After
	public void tearDown() throws Exception {
	}

	@Test
	public void testBitVectorConversions() throws Exception {
		Random rand = new Random();
		byte[] vector = new byte[16];
		rand.nextBytes(vector);
		byte[] output = KryptnosticEngine.testBitVectorConversion(vector);
		assertArrayEquals(vector, output);
	}

	@Test
	public void testBitMatrixConversions() throws Exception {
		Random rand = new Random();
		byte[] matrix = new byte[2048];
		rand.nextBytes(matrix);
		byte[] output = KryptnosticEngine.testBitMatrixConversion(matrix);
		assertArrayEquals(matrix, output);
	}

	@Test
	public void testClientHashFunctionConversions() throws Exception {
		Random rand = new Random();
		byte[] chf = new byte[1060896];
		rand.nextBytes(chf);
		byte[] output = KryptnosticEngine.testClientHashFunctionConversion(chf);
		assertArrayEquals(chf, output);
	}

	@Test
	public void testMetadataAddressCalculations() throws Exception {
		Random rand = new Random();
		byte[] pk = KryptnosticEngine.testPrivateKey();
		byte[] spk = KryptnosticEngine.testSearchPrivateKey();
		byte[] osk = KryptnosticEngine.testObjectSearchKey(spk);
		byte[] oam = KryptnosticEngine.testObjectAddressMatrix(spk);
		byte[] ocm = KryptnosticEngine.testObjectConversionMatrix(spk, oam);
		byte[] token = new byte[16];
		rand.nextBytes(token);
		byte[] clientCalculatedAddress = KryptnosticEngine.testClientMetadataAddress(spk, oam, osk, token);

		byte[] etoken = KryptnosticEngine.testEncryptionFHE(pk, token);
		byte[] eosk = KryptnosticEngine.testEncryptionFHE(pk, osk);
		byte[] chf = KryptnosticEngine.testClientHashFunction(spk, pk);

		KryptnosticEngine ke = new KryptnosticEngine(chf, etoken);
		byte[] serverCalculatedAddress = ke.calculateMetadataAddress(eosk, ocm);

		System.out.print("[");
		for( byte b : serverCalculatedAddress ){
			System.out.print( (int) b );
			System.out.print(",");
		}
		System.out.print("]");
		assertArrayEquals(clientCalculatedAddress, serverCalculatedAddress);
		// assertArrayEquals(token, token);
	}

	@Test
	public void smokeTest() throws Exception {
		Random rand = new Random();
		byte[] clientHashFunc = new byte[16];
		byte[] encObjectSearchToken = new byte[16];
		rand.nextBytes(clientHashFunc);
		rand.nextBytes(encObjectSearchToken);
		KryptnosticEngine kryptnosticEngine = new KryptnosticEngine(clientHashFunc, encObjectSearchToken);
		assertNotNull(kryptnosticEngine.getHandle());
		byte[] encObjectSearchKey = new byte[16];
		byte[] objectConversionMatrix = new byte[16];
		kryptnosticEngine.calculateMetadataAddress(encObjectSearchKey, objectConversionMatrix);
	}
}
