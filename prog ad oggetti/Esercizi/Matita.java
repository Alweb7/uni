public class Matita {
    public static final int minStelo=10;
    public static final int maxStelo=200;
    public static final int maxPunta=5;
    private int stelo;
    private int punta;
    public Matita(int stelo){
        this.stelo=stelo;
        this.punta=maxPunta;
    }

    public int getStelo(){
        return stelo;
    }
    public int getPunta(){
        return punta;
    } 
      
}
