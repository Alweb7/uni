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
    static boolean confronto(int[][] a,int[][] b){
        if (n_row(a)!=n_row(b)) {
            if (n_coluns(a)!=n_coluns(b)||n_coluns(a)==0||n_coluns(b)==0) {
                return false;
            }
            return false;            
        }
        for(int i=0;i<a.length;i++){
            for(int j=0;j<a[i].length;j++){
                if (a[i][j]!=b[i][j]) {
                    return false;                    
                }
            }
        }
        return true;

    }

    public static void main(String[] args) {
        
    }
    
}
