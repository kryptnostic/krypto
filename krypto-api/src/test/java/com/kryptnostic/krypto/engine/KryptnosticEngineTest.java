package com.kryptnostic.krypto.engine;

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
    public void testInitialization() throws Exception {
        KryptnosticEngine eng = new KryptnosticEngine(new byte[0], new byte[0], new byte[0]);
    }
}
