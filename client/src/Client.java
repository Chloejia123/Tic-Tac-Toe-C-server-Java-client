/**
 * This is the file for tic-tac-toe client written in java.
 */

import com.google.gson.Gson;
import com.google.gson.GsonBuilder;


import org.apache.http.HttpEntity;
import org.apache.http.HttpResponse;
import org.apache.http.client.HttpClient;
import org.apache.http.client.methods.HttpGet;
import org.apache.http.client.methods.HttpPost;
import org.apache.http.entity.StringEntity;
import org.apache.http.impl.client.CloseableHttpClient;
import org.apache.http.impl.client.HttpClientBuilder;

import java.io.BufferedInputStream;
import java.io.BufferedReader;
import java.io.InputStream;
import java.io.InputStreamReader;

public class Client {
  private static final String URL_GAME = "http://localhost:1234/game";
  private static final String URL_MOVE = "http://localhost:1234/move";
  private static String whoGoesFirst = "";
  public static Board board;
  public static Board numberBoard;

  public Client() {
    board = new Board();
    numberBoard = new Board();
    numberBoard.boardArray =
        new String[]{"[1]", "[2]", "[3]", "[4]", "[5]", "[6]",
            "[7]", "[8]", "[9]",};
  }

  /**
   * This is the main function
   * @param args is the input arguments, but no args is required.
   */
  public static void main(String[] args) {
    Client client = new Client();
    CloseableHttpClient httpClient = HttpClientBuilder.create().build();
    initializeGame(httpClient);
    client.play(httpClient);
  }

  /**
   * This is a helper method that gets HttpEntity as a string
   *
   * @param entity is the http entity
   * @return a string that contains the info get from the HttpEntity
   */
  private static String getHTTPEntity(HttpEntity entity) {
    StringBuilder completePayload = new StringBuilder();
    try {
      byte[] buffer = new byte[1024];

      if (entity != null) {
        InputStream inputStream = entity.getContent();
        try {
          int bytesRead = 0;
          BufferedInputStream bis = new BufferedInputStream(inputStream);
          while ((bytesRead = bis.read(buffer)) != -1) {
            String chunk = new String(buffer, 0, bytesRead);
            completePayload.append(chunk);
          }
        } catch (Exception e) {
          e.printStackTrace();
        } finally {
          try {
            inputStream.close();
          } catch (Exception ignore) {
          }
        }
      }
    } catch (Exception e) {
      e.printStackTrace();
    }
    return completePayload.toString();
  }


  /**
   * This is a method to initiate a new game for the client.
   *
   * @param httpClient is the httpClient to used to create a game
   */
  private static void initializeGame(HttpClient httpClient) {
    String completePayload = "";
    try {
      HttpGet httpGetRequest = new HttpGet(URL_GAME);
      HttpResponse httpResponse = httpClient.execute(httpGetRequest);
      HttpEntity entity = httpResponse.getEntity();
      completePayload = getHTTPEntity(entity);
    } catch (Exception e) {
      e.printStackTrace();
    }

    Gson gson = new GsonBuilder().create();
    WhoGoFirst obj = gson.fromJson(completePayload, WhoGoFirst.class);
    System.out.println();
    System.out.println(obj.greeting);
    System.out.println(obj.who_go_first + " goes first!");
    whoGoesFirst = obj.who_go_first;
  }


  /**
   * This method let's client play the game with server round by round
   *
   * @param httpClient is the given client to play the game
   */
  private void play(CloseableHttpClient httpClient) {
    if (whoGoesFirst.equals("Client")) {
      String[] boardArray = this.board.getBoard();
      printBoard(boardArray,"play");
    }
    for (int turn = 1; turn <= 9 && !checkWin(); ++turn) {
      if (whoGoesFirst.equals("Server")) {
        if (turn % 2 != 0) {
          serverPlay(httpClient);
          String[] board = this.board.getBoard();
          printBoard(board, "play");
        } else {
          clientPlay(httpClient);
        }
      } else {
        if (turn % 2 != 0) {
          clientPlay(httpClient);
        } else {
          serverPlay(httpClient);
          String[] board = this.board.getBoard();
          printBoard(board,"play");
        }
      }

      if (checkWin()) {
        String[] board = this.board.getBoard();
        printBoard(board,"play");
        return;
      }
    }
    printBoard(this.board.boardArray,"play");
    System.out.println("Game Over! You draw with the computer O.O!");
    return;
  }

  /**
   * Client to make a move
   *
   * @param httpClient is the given client that plays the game
   */
  private void clientPlay(CloseableHttpClient httpClient) {
    String[] boardArray = this.board.getBoard();
    boolean isValid = false;
    while (!isValid) {
      System.out.println("Please enter a number between 1 - 9 " +
          "to place your mark");
      printBoard(this.numberBoard.boardArray, "number");
      BufferedReader stdIn = new BufferedReader(
          new InputStreamReader(System.in));
      try {
        String move = stdIn.readLine();
        int moveInt = Integer.parseInt(move);
        if (move != null && move.matches("[1-9]")
            && boardArray[moveInt - 1].equals("[ ]")) {
          changeBoard(boardArray, move);
          isValid = true;
        } else {
          System.out.println("The input is not valid. ");
          System.out.println("Please enter between 1-9 of an empty cell.\n");
        }
      } catch (Exception e) {
        e.printStackTrace();
      }
    }
  }


  /**
   * To update the board based on each client move.
   *
   * @param board is the playBoard
   * @param move  is the place that the client wants to place a mark
   */
  private void changeBoard(String[] board, String move) {
    int NextMove = Integer.parseInt(move) - 1;
    board[NextMove] = "[O]";
    System.out.println("After server played:");
  }


  /**
   * Sever to take a move
   *
   * @param httpClient is the given client to play with server
   */
  private void serverPlay(CloseableHttpClient httpClient) {
    // Send the board to server to take move
    HttpEntity entity = null;
    try {
      // Convert that object to JSON
      Gson gson = new GsonBuilder().create();
      String jsonString = gson.toJson(board);

      // Setup the Post request
      HttpPost httpPostRequest = new HttpPost(URL_MOVE);
      // Set the JSON header
      httpPostRequest.setHeader("Content-type",
          "application/json");
      // Set the payload of the request
      httpPostRequest.setEntity(new StringEntity(jsonString));
      // Execute the request
      HttpResponse httpResponse = httpClient.execute(httpPostRequest);
      entity = httpResponse.getEntity();
    } catch (Exception e) {
      e.printStackTrace();
    }
    getBoardFromServer(entity);
  }


  /**
   * Update the board after the server has made a move
   */
  private void getBoardFromServer(HttpEntity entity) {
    String completePayload = "";
    completePayload = getHTTPEntity(entity);
    Gson gson = new GsonBuilder().create();
    Board newBoard = gson.fromJson(completePayload, Board.class);
    board = newBoard;
  }


  /**
   * To check if there is a winner
   *
   * @return Corresponding message that indicate a winner or return false
   * for no winner yet
   */
  private boolean checkWin() {
    String[] checkBoard = board.boardArray;
    int check[][] = {{0, 1, 2}, {3, 4, 5}, {6, 7, 8}, {0, 3, 6}
        , {1, 4, 7}, {2, 5, 8}, {0, 4, 8}, {2, 4, 6}};
    for (int i = 0; i < 8; ++i) {
      if (!checkBoard[check[i][0]].equals(" ")
          && checkBoard[check[i][0]].equals(checkBoard[check[i][1]])
          && checkBoard[check[i][0]].equals(checkBoard[check[i][2]])) {
        if (checkBoard[check[i][0]].equals("[O]")) {
          System.out.println("Congrats! You WIN!!! ^o^");
          System.exit(0);
          return true;
        } else if (checkBoard[check[i][0]].equals("[X]")) {
          System.out.println("Oh no! You LOSE! TAT");
          System.exit(0);
          return true;
        }
      }
    }
    return false;
  }


  /**
   * To print the board to the console
   *
   * @param boardArray is the String array that stores the play status
   */
  public static void printBoard(String[] board, String type) {
    if (type.equalsIgnoreCase("number")) {
      System.out.println("*** Number Board ***");
    } else {
      System.out.println("*** Current Play Board ***");
    }
    int i = 0;
    for (int row = 0; row < 3; row++) {
      System.out.println("+---+---+---+");
      System.out.print('|');
      for (int col = 0; col < 3; col++) {
        System.out.print(" " + board[i++].charAt(1) + " ");
        System.out.print('|');
      }
      System.out.println();
    }
    System.out.println("+---+---+---+");
  }


  class WhoGoFirst {
    private String greeting;
    private String who_go_first;

    @java.lang.Override
    public java.lang.String toString() {
      return "WhoGoFirst{" +
          "greeting='" + greeting + '\'' +
          ", who_go_first='" + who_go_first + '\'' +
          '}';
    }
  }

  class Board {
    public String[] boardArray;

    public Board() {
      boardArray = new String[9];
      for (int i = 0; i < boardArray.length; i++) {
        boardArray[i] = "[ ]";
      }
    }

    public String[] getBoard() {
      return this.boardArray;
    }
  }
}
