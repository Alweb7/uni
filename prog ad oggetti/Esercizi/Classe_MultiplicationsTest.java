public class Classe_MultiplicationsTest {

    static int passed = 0;
    static int failed = 0;

    static void assertEquals(int expected, int actual, String label) {
        if (expected == actual) {
            System.out.println("  [OK] " + label);
            passed++;
        } else {
            System.out.println("  [FAIL] " + label +
                " → atteso " + expected + ", ottenuto " + actual);
            failed++;
        }
    }

    static void assertTrue(boolean condition, String label) {
        assertEquals(1, condition ? 1 : 0, label);
    }

    // ── Newarray() ────────────────────────────────────────────────────────────

    static void test_dimensioni() {
        System.out.println("Newarray() - dimensioni:");
        int[][] a = Classe_Multiplications.Newarray();
        assertEquals(10, a.length,    "10 righe");
        assertEquals(10, a[0].length, "10 colonne");
    }

    static void test_angoli() {
        System.out.println("Newarray() - angoli:");
        int[][] a = Classe_Multiplications.Newarray();
        assertEquals(1,   a[0][0], "a[0][0] = 1x1 = 1");
        assertEquals(10,  a[0][9], "a[0][9] = 1x10 = 10");
        assertEquals(10,  a[9][0], "a[9][0] = 10x1 = 10");
        assertEquals(100, a[9][9], "a[9][9] = 10x10 = 100");
    }

    static void test_prima_riga() {
        System.out.println("Newarray() - prima riga (tavola del 1):");
        int[][] a = Classe_Multiplications.Newarray();
        for (int j = 0; j < 10; j++)
            assertEquals(j + 1, a[0][j], "a[0][" + j + "] = " + (j + 1));
    }

    static void test_prima_colonna() {
        System.out.println("Newarray() - prima colonna (tavola del 1):");
        int[][] a = Classe_Multiplications.Newarray();
        for (int i = 0; i < 10; i++)
            assertEquals(i + 1, a[i][0], "a[" + i + "][0] = " + (i + 1));
    }

    static void test_diagonale() {
        System.out.println("Newarray() - diagonale principale (quadrati perfetti):");
        int[][] a = Classe_Multiplications.Newarray();
        for (int i = 0; i < 10; i++)
            assertEquals((i+1)*(i+1), a[i][i], "a["+i+"]["+i+"] = "+(i+1)+"^2 = "+((i+1)*(i+1)));
    }

    static void test_simmetria() {
        System.out.println("Newarray() - simmetria a[i][j] == a[j][i]:");
        int[][] a = Classe_Multiplications.Newarray();
        boolean ok = true;
        for (int i = 0; i < 10 && ok; i++)
            for (int j = 0; j < 10 && ok; j++)
                if (a[i][j] != a[j][i]) ok = false;
        assertTrue(ok, "matrice simmetrica");
    }

    static void test_valori_interni() {
        System.out.println("Newarray() - valori interni:");
        int[][] a = Classe_Multiplications.Newarray();
        assertEquals(6,  a[1][2], "a[1][2] = 2x3 = 6");
        assertEquals(35, a[4][6], "a[4][6] = 5x7 = 35");
        assertEquals(72, a[7][8], "a[7][8] = 8x9 = 72");
        assertEquals(45, a[4][8], "a[4][8] = 5x9 = 45");
    }

    // ── prodotto(b, c) ────────────────────────────────────────────────────────

    static void test_prodotto_base() {
        System.out.println("prodotto() - casi base:");
        assertEquals(1,   Classe_Multiplications.prodotto(1,  1),  "1x1 = 1");
        assertEquals(100, Classe_Multiplications.prodotto(10, 10), "10x10 = 100");
        assertEquals(10,  Classe_Multiplications.prodotto(1,  10), "1x10 = 10");
        assertEquals(10,  Classe_Multiplications.prodotto(10, 1),  "10x1 = 10");
    }

    static void test_prodotto_misti() {
        System.out.println("prodotto() - valori misti:");
        assertEquals(12, Classe_Multiplications.prodotto(3, 4),  "3x4 = 12");
        assertEquals(42, Classe_Multiplications.prodotto(6, 7),  "6x7 = 42");
        assertEquals(18, Classe_Multiplications.prodotto(2, 9),  "2x9 = 18");
        assertEquals(50, Classe_Multiplications.prodotto(5, 10), "5x10 = 50");
    }

    static void test_prodotto_commutativo() {
        System.out.println("prodotto() - proprietà commutativa:");
        int[][] coppie = {{3,7},{2,8},{4,9},{5,6}};
        for (int[] c : coppie) {
            int ab = Classe_Multiplications.prodotto(c[0], c[1]);
            int ba = Classe_Multiplications.prodotto(c[1], c[0]);
            assertEquals(ab, ba, "prodotto(" + c[0] + "," + c[1] + ") == prodotto(" + c[1] + "," + c[0] + ")");
        }
    }

    static void test_prodotto_coerente_con_newarray() {
        System.out.println("prodotto() - coerente con Newarray():");
        int[][] a = Classe_Multiplications.Newarray();
        for (int b = 1; b <= 10; b++)
            for (int c = 1; c <= 10; c++)
                assertEquals(a[b-1][c-1], Classe_Multiplications.prodotto(b, c),
                    "prodotto("+b+","+c+") == a["+(b-1)+"]["+(c-1)+"]");
    }

    // ── main ──────────────────────────────────────────────────────────────────

    public static void main(String[] args) {
        System.out.println("=== TEST Classe_Multiplications ===\n");

        test_dimensioni();
        test_angoli();
        test_prima_riga();
        test_prima_colonna();
        test_diagonale();
        test_simmetria();
        test_valori_interni();
        test_prodotto_base();
        test_prodotto_misti();
        test_prodotto_commutativo();
        test_prodotto_coerente_con_newarray();

        System.out.println("\n===================================");
        System.out.println("Risultato: " + passed + " OK  |  " + failed + " FAIL");
        if (failed == 0) System.out.println("Tutti i test superati!");
    }
}
