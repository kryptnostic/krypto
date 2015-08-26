package com.kryptnostic.krypto.engine;

import java.util.Random;

import org.junit.After;
import org.junit.AfterClass;
import org.junit.Before;
import org.junit.BeforeClass;
import org.junit.Test;

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
    public void smokeTest() throws Exception {
        Random rand = new Random();
        byte[] clientHashFunc = new byte[16];
        byte[] encObjectSearchToken = new byte[16];
        rand.nextBytes( clientHashFunc );
        rand.nextBytes( encObjectSearchToken );
        new KryptnosticEngine( clientHashFunc, encObjectSearchToken );
    }
}
