#include "GameController.h"
#include "ChessException.h"
#include <iostream>
#include <sstream>
#include <algorithm>
#include <cctype>

GameController::GameController() : isRunning(true) {}


void GameController::run() {
    while (isRunning) {
        view.clearScreen();
        view.displayMenu();
        std::string luaChon = view.promptInput("  Nhap lua chon cua ban (1-5): ");

        if (luaChon == "1") {
            // Bat dau van moi
            game.reset();
            gameLoop();

        } else if (luaChon == "2") {
            // Tai van da luu tu file
            std::string tenFile = view.promptInput("  Nhap duong dan file luu (mac dinh: savegame.txt): ");
            if (tenFile.empty()) tenFile = "savegame.txt";
            try {
                game.loadFromFile(tenFile);
                view.displayMessage("Tai game thanh cong! Bat dau choi...", false);
                waitForEnter();
                gameLoop();
            } catch (const ChessException& e) {
                view.displayMessage(e.what(), true);
                waitForEnter();
            }

        } else if (luaChon == "3") {
            // Hien thi huong dan
            view.displayHelp();
            waitForEnter();

        } else if (luaChon == "4") {
            // Hien thi lich su nuoc di
            view.displayHistory(game.getMoveHistory());
            waitForEnter();

        } else if (luaChon == "5") {
            // Thoat chuong trinh
            isRunning = false;
            view.displayMessage("Cam on ban da choi! Tam biet.", false);

        } else {
            view.displayMessage("Lua chon khong hop le. Vui long chon tu 1 den 5.", true);
            waitForEnter();
        }
    }
}


void GameController::gameLoop() {
    bool dangChoi = true;

    while (dangChoi) {
        // Kiem tra dieu kien thang cuoc: Vua bi tieu diet
        if (game.isKingCaptured(Color::WHITE)) {
            view.clearScreen();
            view.displayBoard(game.getBoard());
            view.displayMessage("BEN DEN THANG! Quan Vua ben Trang da bi tieu diet!", false);
            waitForEnter();
            break;
        }
        if (game.isKingCaptured(Color::BLACK)) {
            view.clearScreen();
            view.displayBoard(game.getBoard());
            view.displayMessage("BEN TRANG THANG! Quan Vua ben Den da bi tieu diet!", false);
            waitForEnter();
            break;
        }

        // Hien thi ban co va thong tin luot di
        view.clearScreen();
        view.displayBoard(game.getBoard());

        Color luot = game.getCurrentTurn();
        std::string tenLuot = (luot == Color::WHITE)
            ? "\033[1;34m  >> Luot di cua TRANG (Chu HOA, Mau xanh duong)\033[0m"
            : "\033[1;31m  >> Luot di cua DEN (chu thuong, Mau do)\033[0m";
        std::cout << tenLuot << "\n";
        std::cout << "     (Nhap 'save' de luu | 'exit' de thoat ve menu)\n\n";

        // Nhan nuoc di tu nguoi choi (qua View)
        std::string input = view.promptInput("  Nuoc di cua ban (vi du: e2 e4): ");

        // Chuyen thanh chu thuong de kiem tra lenh
        std::string inputThuong = input;
        std::transform(inputThuong.begin(), inputThuong.end(), inputThuong.begin(), ::tolower);
        std::string inputGon = inputThuong;
        inputGon.erase(std::remove_if(inputGon.begin(), inputGon.end(), ::isspace), inputGon.end());

        // Xu ly cac lenh dac biet
        if (inputGon == "exit") {
            std::string xacNhan = view.promptInput("  Ban co chac muon thoat ve menu chinh? (y/n): ");
            if (xacNhan == "y" || xacNhan == "Y") dangChoi = false;
            continue;
        }

        if (inputGon == "save") {
            std::string tenFile = view.promptInput("  Ten file luu (mac dinh: savegame.txt): ");
            if (tenFile.empty()) tenFile = "savegame.txt";
            try {
                game.saveToFile(tenFile);
                view.displayMessage("Da luu van dau vao file '" + tenFile + "'.", false);
            } catch (const ChessException& e) {
                view.displayMessage(e.what(), true);
            }
            waitForEnter();
            continue;
        }

        // Phan tich nuoc di tu chuoi nhap vao
        Position oXuatPhat, oDich;
        if (!parseMove(input, oXuatPhat, oDich)) {
            view.displayMessage("Dinh dang sai! Vui long nhap theo dang 'e2 e4'.", true);
            waitForEnter();
            continue;
        }

        // Gui nuoc di den Model - bat ngoai le va hien thi thong bao qua View
        try {
            // DA HINH luc chay xay ra ben trong playMove()
            game.playMove(oXuatPhat, oDich);

        } catch (const InvalidMoveException& e) {
            view.displayMessage(e.what(), true);
            waitForEnter();
        } catch (const ChessException& e) {
            view.displayMessage(std::string("Loi co vua: ") + e.what(), true);
            waitForEnter();
        } catch (const std::exception& e) {
            view.displayMessage(std::string("Loi he thong: ") + e.what(), true);
            waitForEnter();
        }
    }
}


bool GameController::parseMove(const std::string& input, Position& start, Position& end) const {
    std::stringstream ss(input);
    std::string s1, s2;
    if (!(ss >> s1 >> s2)) return false;
    if (s1.length() != 2 || s2.length() != 2) return false;

    char cot1 = std::tolower(s1[0]);
    char hang1 = s1[1];
    char cot2 = std::tolower(s2[0]);
    char hang2 = s2[1];

    // Kiem tra ky tu hop le: cot a-h, hang 1-8
    if (cot1 < 'a' || cot1 > 'h' || hang1 < '1' || hang1 > '8') return false;
    if (cot2 < 'a' || cot2 > 'h' || hang2 < '1' || hang2 > '8') return false;

    start = Position(hang1 - '1', cot1 - 'a');
    end   = Position(hang2 - '1', cot2 - 'a');
    return true;
}

void GameController::waitForEnter() const {
    std::cout << "\n  Nhan Enter de tiep tuc...";
    std::string tg;
    std::getline(std::cin, tg);
}
