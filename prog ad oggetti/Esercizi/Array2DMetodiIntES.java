public class Array2DMetodiIntES {
    static int n_row(int[][] a){
        return a.length; //numero righe

    }
    static int n_coluns(int[][] a){
        for(int i=0;i<a.length-1;i++){
            if (a[i].length!=a[i+1].length) {
                return 0;
            }
        }
        return a[0].length; //numero colonne
    }
    static void stamp_array(int[][] a){
        for(int i=0;i<a.length;i++){
            for (int is : a[i]) {
                System.out.println(is);
            }
        }
    }

    static int sum_array(int[][] a){
        int sum=0;
        for(int i=0;i<a.length;i++){
            for (int is : a[i]) {
                sum+=is;
            }
        }
        return sum;
    }
    static boolean confronto(int[][] a, int[][] b){
        if (n_row(a) != n_row(b)) return false;
        if (n_coluns(a) != n_coluns(b)) return false;
        for(int i=0;i<a.length;i++){
            for(int j=0;j<a[i].length;j++){
                if (a[i][j]!=b[i][j]) {
                    return false;
                }
            }
        }
        return true;
    }
    static boolean found_e(int[][] a,int e){
        for (int[] is : a) {
            for (int n : is) {
                if (n==e) return true;
            }
        }
        return false;
    }
    static int max_value(int[][] a) {
        int max= a[0][0];
        for (int[] is : a) {
            for (int n : is) {
                if(max<n) max=n;
            }
        }
        return max;   
        
    }

    public static void main(String[] args) {
        int[][] a = {{1,2,3},{4,5,6},{7,8,9}};
        int[][] b = {{1,2,3},{4,5,6},{7,8,9}};
        int[][] c = {{1,2,3},{4,5,6},{7,8,0}};
        int[][] d = {{1,2},{3,4}};

        System.out.println("n_row: " + n_row(a));           // 3
        System.out.println("n_coluns: " + n_coluns(a));     // 3

        System.out.println("stamp_array:");
        stamp_array(a);

        System.out.println("sum_array: " + sum_array(a));   // 45

        System.out.println("confronto a==b: " + confronto(a, b)); // true
        System.out.println("confronto a==c: " + confronto(a, c)); // false (elemento diverso)
        System.out.println("confronto a==d: " + confronto(a, d)); // false (righe diverse)

        System.out.println("found_e 5: " + found_e(a, 5));        // true
        System.out.println("found_e 0: " + found_e(a, 0));        // false

        System.out.println("max_value: " + max_value(a));         // 9
        int[][] negativi = {{-5,-2},{-8,-1}};
        System.out.println("max_value negativi: " + max_value(negativi)); // -1
    }
    
}
