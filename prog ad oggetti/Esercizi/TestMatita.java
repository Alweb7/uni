class Matita {
    public static final int minStelo=10;
    public static final int maxStelo=200;
    public static final int maxPunta=5;
    private int stelo;
    private int punta;
    public Matita(int stelo){
        this.stelo=stelo;
        this.punta=maxPunta;
    }
    public boolean disegna(){
        if (this.punta>1) {
            this.punta--;
            return true;           
        }
       return false;
    }
    public boolean tempera(){
        if(this.stelo>minStelo){
            if (this.punta<maxPunta) {
                this.punta++;
                this.stelo--;                
            }else{
                this.stelo--;
            }
            return true;
        }
        return false;

    }
    public int getStelo(){
        return stelo;
    }
    public int getPunta(){
        return punta;
    }
    
      
}

public class TestMatita {

    public static void main(String[] args){ 
        Matita m = new Matita(Matita.maxStelo); 
        int s = m.getStelo(), p = m.getPunta();
        System.out.println("Matita di stelo " + s + " e punta " + p); 
        int numDisegni = p+2;
        System.out.println("Disegno per " + numDisegni + " volte:"); 
        System.out.println("dopo " + p + " volte il disegno fallisce");
        for (int i = 0; i < numDisegni; i++)
            System.out.println(" Successo disegno n."+i+" = "+m.disegna());
        System.out.println("Tempero di 1mm la matita"); m.tempera();
        System.out.println(" nuova lunghezza punta = " + m.getPunta());
        System.out.println(" nuova lunghezza stelo = " + m.getStelo());
        System.out.println("Stampo la matita m. Ottengo \"Matita@\" seguito dall'indirizzo dell'oggetto (in esadecimale): " + m);
    }

}