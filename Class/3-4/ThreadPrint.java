public class ThreadPrint extends Thread {
  public ThreadPrint(String name) {
    super(name);
  }

  public void run() {
    for (int k = 1; k < 128; k++) {
      System.out.println(this.getName() + ": " + k);
    }
  }

  public static void main(String[] args) {
    ThreadPrint dori = new ThreadPrint("dori");
    ThreadPrint mina = new ThreadPrint("mina");
    dori.start();
    mina.start();
  }
}
