#include "Game.h"
#include "Pieces.h"
#include "ChessException.h"
#include <fstream>
#include <sstream>
#include <cctype>

Game::Game() {
    reset();
}

void Game::reset() {
    board.initialize();
    currentTurn = Color::WHITE; // Ben Trang di truoc
    moveHistory.clear();
}


bool Game::isKingCaptured(Color mauVua) const {
    for (int r = 0; r < 8; ++r) {
        for (int c = 0; c < 8; ++c) {
            Piece* p = board.getPiece(r, c);
            if (p && p->getType() == PieceType::KING && p->getColor() == mauVua)
                return false;
        }
    }
    return true;
}


static std::string chuyenSangKyHieu(const Position& pos) {
    return std::string(1, 'a' + pos.col) + std::string(1, '1' + pos.row);
}


void Game::playMove(const Position& start, const Position& end) {
    // Kiem tra toa do hop le
    if (!start.isValid() || !end.isValid())
        throw InvalidMoveException("Toa do o co nam ngoai pham vi ban co (a1 - h8).");

    if (start == end)
        throw InvalidMoveException("O xuat phat va o dich khong the giong nhau.");


    Piece* p = board.getPiece(start);
    if (p == nullptr)
        throw InvalidMoveException("Khong tim thay quan co tai o xuat phat.");


    if (p->getColor() != currentTurn) {
        std::string luotHienTai = (currentTurn == Color::WHITE) ? "TRANG" : "DEN";
        throw InvalidMoveException("Khong phai luot cua ban! Luot hien tai la: " + luotHienTai + ".");
    }


    if (!p->isValidMove(start, end, board))
        throw InvalidMoveException("Nuoc di nay sai luat hoac bi chan duong.");


    std::string ghiNhuoc = (currentTurn == Color::WHITE ? "Trang: " : "Den: ")
                         + chuyenSangKyHieu(start) + " -> " + chuyenSangKyHieu(end);

    Piece* quanDich = board.getPiece(end);
    if (quanDich != nullptr)
        ghiNhuoc += " (An " + std::string(1, std::toupper(quanDich->getSymbol())) + ")";

    moveHistory.push_back(ghiNhuoc);


    board.executeMove(start, end);


    switchTurn();
}


void Game::switchTurn() {
    currentTurn = (currentTurn == Color::WHITE) ? Color::BLACK : Color::WHITE;
}


void Game::saveToFile(const std::string& filepath) const {
    std::ofstream file(filepath);
    if (!file.is_open())
        throw FileIOException("Khong the mo file de ghi: " + filepath);


    file << (currentTurn == Color::WHITE ? "W" : "B") << "\n";


    for (int r = 7; r >= 0; --r) {
        for (int c = 0; c < 8; ++c) {
            Piece* p = board.getPiece(r, c);
            file << (p ? p->getSymbol() : '.');
        }
        file << "\n";
    }

    // Ghi lich su nuoc di
    file << moveHistory.size() << "\n";
    for (const auto& nuoc : moveHistory)
        file << nuoc << "\n";

    file.close();
}

// Doc trang thai van dau tu file (dam bao toan ven du lieu)
void Game::loadFromFile(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open())
        throw FileIOException("Khong the mo file luu: " + filepath);

    // Doc luot di
    std::string chuoiLuot;
    if (!std::getline(file, chuoiLuot))
        throw FileIOException("File bi hong: thieu thong tin luot di.");

    Color luotTam;
    if (chuoiLuot == "W")      luotTam = Color::WHITE;
    else if (chuoiLuot == "B") luotTam = Color::BLACK;
    else throw FileIOException("File bi hong: gia tri luot di khong hop le.");

    // Doc trang thai ban co (8 dong tu hang 7 xuong hang 0)
    Board banTam;
    for (int r = 7; r >= 0; --r) {
        std::string dong;
        if (!std::getline(file, dong) || (int)dong.size() < 8)
            throw FileIOException("File bi hong: hang " + std::to_string(r + 1) + " bi thieu hoac qua ngan.");

        for (int c = 0; c < 8; ++c) {
            char ch = dong[c];
            if (ch == '.') {
                banTam.setPiece(Position(r, c), nullptr);
                continue;
            }
            Color mau = std::isupper(ch) ? Color::WHITE : Color::BLACK;
            char kyHieu = std::tolower(ch);
            Piece* quanMoi = nullptr;
            switch (kyHieu) {
                case 'p': quanMoi = new Pawn(mau,   Position(r, c)); break;
                case 'r': quanMoi = new Rook(mau,   Position(r, c)); break;
                case 'n': quanMoi = new Knight(mau, Position(r, c)); break;
                case 'b': quanMoi = new Bishop(mau, Position(r, c)); break;
                case 'q': quanMoi = new Queen(mau,  Position(r, c)); break;
                case 'k': quanMoi = new King(mau,   Position(r, c)); break;
                default:
                    throw FileIOException(std::string("Ky tu quan co khong hop le trong file: '") + ch + "'.");
            }
            banTam.setPiece(Position(r, c), quanMoi);
        }
    }

    // Doc lich su nuoc di
    std::vector<std::string> lichSuTam;
    std::string chuoiSo;
    if (std::getline(file, chuoiSo)) {
        int soNuoc = 0;
        std::stringstream ss(chuoiSo);
        if (ss >> soNuoc) {
            for (int i = 0; i < soNuoc; ++i) {
                std::string nuoc;
                if (std::getline(file, nuoc))
                    lichSuTam.push_back(nuoc);
            }
        }
    }

    file.close();


    board       = banTam;
    currentTurn = luotTam;
    moveHistory = lichSuTam;
}
