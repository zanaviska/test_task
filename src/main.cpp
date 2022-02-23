#include <bit>
#include <cmath>
#include <fstream>
#include <future>
#include <iostream>
#include <optional>
#include <set>
#include <thread>
#include <vector>

// exact realization was taken from https://github.com/flexxxxer/concurrent-stack
class atomic_stack
{
public:
    atomic_stack() : top_(nullptr) {}

    void push(const std::string &value) noexcept
    {
        stack_node *new_node = new stack_node(value, this->top_.load());

        while (!this->top_.compare_exchange_weak(new_node->next, new_node))
            ;
    }
    std::optional<std::string> pop() noexcept
    {
        stack_node *top = this->top_.load();

        if (top == nullptr) return std::nullopt;

        stack_node *new_front = top->next;

        while (!this->top_.compare_exchange_weak(top, new_front))
        {
            if (top == nullptr) return std::nullopt;

            new_front = top->next;
        }

        std::string value = std::move(top->value);
        delete top;

        return value;
    }
    [[nodiscard]] bool is_empty() const noexcept { return this->top_.load() == nullptr; }

private:
    struct stack_node
    {
        std::string value;
        stack_node *next;

        explicit stack_node(const std::string &value) : value(value), next(nullptr) {}
        explicit stack_node(const std::string &value, stack_node *next) : value(value), next(next)
        {
        }
    };

    std::atomic<stack_node *> top_;
};

// 
std::set<std::string> result_set_calculator(size_t tree_size, atomic_stack &as,
                                            std::atomic_bool &finished_reading)
{
    std::set<std::string> res;

    std::vector<std::future<std::set<std::string>>> child_calculators;
    size_t child_thread_count = std::ceil(log2(tree_size));
    child_calculators.reserve(child_thread_count);

    while (tree_size > 1)
    {
        auto old_tree_size = tree_size;
        tree_size /= 2;
        int x = tree_size;
        child_calculators.emplace_back(
            std::async([tree_size, &as, &finished_reading]
                       { return result_set_calculator(tree_size, as, finished_reading); }));
        tree_size = old_tree_size - tree_size;
    }


    while (!finished_reading || !as.is_empty())
        if (auto word = as.pop(); word.has_value()) res.insert(std::move(word.value()));

    for (auto &i : child_calculators)
    {
        auto child_library = i.get();
        res.insert(std::make_move_iterator(child_library.begin()),
                   std::make_move_iterator(child_library.end()));
    }

    return res;
}

int main(int argc, char* argv[])
{
    std::string path_to_file = "text.txt";
    if (argc > 1) path_to_file = argv[1];

    std::ifstream fin(path_to_file);
    size_t thread_count = std::thread::hardware_concurrency();
    
    std::string word;

    atomic_stack as;
    std::atomic_bool finished_reading = false;

    std::future<std::set<std::string>> ftr_library(
        std::async([thread_count, &as, &finished_reading]
                   { return result_set_calculator(thread_count, as, finished_reading); }));

    while (fin >> word)
        as.push(word);

    finished_reading = true;

    std::cout << ftr_library.get().size();
}